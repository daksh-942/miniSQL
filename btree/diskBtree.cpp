#include ".\diskBtree.h"

DiskBtree :: DiskBtree(Pager* pager){
    this->pager=pager;
    
    if (pager->get_page_count()==0){

        root_page=pager->allocate_page();  //0 id 

        char BUFFER[PAGE_SIZE]={0};
        BtreePage page(BUFFER);

        page.set_leaf(true);
        page.set_key_count(0);
        page.set_parent(0);
        page.set_next(0);

        pager->write_page(root_page,BUFFER);
        
        uint32_t METABUFF=pager->allocate_page();  //page id 1 is reserved for metadata
        char METADATA[PAGE_SIZE]={0};
        memcpy(METADATA,&root_page,4);
        pager->write_page(METABUFF,METADATA);
    }
    else{
        char METADATA[PAGE_SIZE]={0};
        pager->read_page(1,METADATA);
        memcpy(&root_page,METADATA,4);
    }
}


std :: pair<uint32_t,uint16_t> DiskBtree :: search(int key){
    uint32_t current = root_page;

    char BUFFER[PAGE_SIZE]={0};

    while(true){

        pager->read_page(current,BUFFER);
        BtreePage page(BUFFER);

        int* keys=page.get_keys();
        uint16_t count=page.get_key_count();

        if(page.is_leaf()){
            auto* vals=page.values();
            for(int i=0;i<count;i++){
                if(keys[i]==key){
                    return vals[i];
                }
            }
            return {0,0};
        }

        int i=0;
        while(i<count && keys[i]<=key){
            i++;
        }
        uint32_t* children=page.get_children();
        current=children[i];
    }
}


void DiskBtree :: insert(uint32_t key, std ::pair<uint32_t, uint16_t> value){
    insert_recursive(key,value,root_page);
}

void DiskBtree :: insert_recursive(int key, std ::pair<uint32_t, uint16_t> value, uint32_t page_id){
    
    char BUFFER[PAGE_SIZE];
    pager->read_page(page_id,BUFFER);
    BtreePage page(BUFFER);

    // "Every B+ tree node is stored as a page on disk. Whenever I need to access a node, I use the Pager to load that page into a buffer. I then wrap the buffer with a BtreePage object so I can manipulate it using high-level methods instead of raw bytes. Once the operation is complete, I write the modified buffer back to disk if any changes were made. This separation of storage (Pager) and page logic (BtreePage) makes the design clean and efficient."

    uint16_t count = page.get_key_count();
    int* keys=page.get_keys();
    
    if(page.is_leaf()){
        if(count < page.MAX_KEYS){
            auto* vals=page.values();

            int i=count-1;
            while(i>=0 && key<=keys[i]){
                keys[i+1]=keys[i];
                vals[i+1]=vals[i];
                i--;
            }

            keys[i+1]=key;
            vals[i+1]=value;

            page.set_key_count(count+1);
            pager->write_page(page_id,BUFFER);
        }
        else{
            split_leaf(page_id,key,value);
        }
    }
    else{
        int i=0;
        while(i<count && key>=keys[i]) i++;
        insert_recursive(key,value,page.get_children()[i]);
    }
}


void DiskBtree ::  split_leaf(uint32_t page_id,int key,std::pair<uint32_t,uint16_t> value){

    char left[PAGE_SIZE];
    pager->read_page(page_id,left);
    BtreePage left_page(left);
    int* l_keys=left_page.get_keys();
    auto* l_values=left_page.values();

    char right[PAGE_SIZE]={0};
    uint32_t right_id= pager->allocate_page();
    BtreePage right_page(right);
    right_page.set_leaf(true);
    right_page.set_parent(left_page.parent());
    int* r_keys=right_page.get_keys();
    auto* r_values=right_page.values();


    int temp_key[left_page.ORDER];
    std :: pair<uint32_t,uint16_t> temp_val[left_page.ORDER];

    for(int i=0;i<left_page.MAX_KEYS;i++){
        temp_key[i]=l_keys[i];
        temp_val[i]=l_values[i];
    }

    int i=left_page.MAX_KEYS-1;
    while(i>=0 && key<=temp_key[i]){
        temp_key[i+1]=temp_key[i];
        temp_val[i+1]=temp_val[i];
        i--;
    }

    temp_key[i+1]=key;
    temp_val[i+1]=value;


    uint16_t mid=left_page.ORDER/2;

    left_page.set_key_count(mid);
    for(int i=0;i<mid;i++){
        l_keys[i]=temp_key[i];
        l_values[i]=temp_val[i];
    }

    right_page.set_key_count(mid);
    for(int i=0;i<mid;i++){
        r_keys[i]=temp_key[i+mid];
        r_values[i]=temp_val[i+mid];
    }

    right_page.set_next(left_page.get_next());
    left_page.set_next(right_id);

    pager->write_page(right_id,right);
    pager->write_page(page_id,left);

    insert_into_parent(page_id,r_keys[0],right_id);

}

void DiskBtree :: split_internal(uint32_t page_id){

    char left[PAGE_SIZE];
    pager->read_page(page_id,left);
    BtreePage left_page(left);
    int* l_keys=left_page.get_keys();
    auto* l_child=left_page.get_children();  

    char right[PAGE_SIZE]={0};
    uint32_t right_id= pager->allocate_page();
    BtreePage right_page(right);
    right_page.set_leaf(false);
    right_page.set_parent(left_page.parent());
    int* r_keys=right_page.get_keys();
    auto* r_child=right_page.get_children();

    int mid=left_page.ORDER/2;
    int promote_key=l_keys[mid];
    int l_count=left_page.get_key_count();

    right_page.set_key_count(l_count-mid-1);
    for(int i=mid+1;i<l_count;i++){
        r_keys[i-mid-1]=l_keys[i];
    }



    for(int i=mid+1;i<=l_count;i++){
        //here we are setting the parent of the child nodes of the right node to the right node itself
        r_child[i-mid-1]=l_child[i];
        char cbuf[PAGE_SIZE];
        pager->read_page(l_child[i], cbuf);

        BtreePage child(cbuf);
        child.set_parent(right_id);

        pager->write_page(l_child[i], cbuf);
    }


    left_page.set_key_count(mid);

    pager->write_page(right_id,right);
    pager->write_page(page_id,left);  //persist the changes made to the left page after splitting

    insert_into_parent(page_id,promote_key,right_id);
}

void DiskBtree :: insert_into_parent(uint32_t left_page_id,int key, uint32_t right_page_id){
    char left[PAGE_SIZE];
    pager->read_page(left_page_id,left);
    BtreePage left_page(left);

    uint32_t parent_id=left_page.parent();

    char right[PAGE_SIZE];
    pager->read_page(right_page_id,right);
    BtreePage right_page(right);
    
    if (parent_id==0){ //why parent
        
        //when we are splliting the root node we need to create a new root node and set the left and right child of the new root node to the left and right child of the old root node respectively
        uint32_t new_root_id=pager->allocate_page();

        char new_root[PAGE_SIZE]={0};
        BtreePage new_root_page(new_root);
        new_root_page.set_key_count(1);
        new_root_page.set_parent(0);
        new_root_page.set_leaf(false);

        new_root_page.get_keys()[0]=key;
        new_root_page.get_children()[0]=left_page_id;
        new_root_page.get_children()[1]=right_page_id;
        pager->write_page(new_root_id,new_root);

        left_page.set_parent(new_root_id);
        pager->write_page(left_page_id,left);
        right_page.set_parent(new_root_id);
        pager->write_page(right_page_id,right);

        root_page=new_root_id;
        
        char meta_buf[PAGE_SIZE]={0};
        memcpy(meta_buf,&root_page,4);
        pager->write_page(1,meta_buf);
        return ;
    }

    char parent[PAGE_SIZE];
    pager->read_page(parent_id,parent);
    BtreePage parent_page(parent);
    uint16_t count=parent_page.get_key_count();
    int* keys=parent_page.get_keys();
    uint32_t* child=parent_page.get_children();


    int i=count-1;
    while (i>=0 && key<=keys[i]){
        keys[i+1]=keys[i];
        child[i+2]=child[i+1];
        i--;
    }

    keys[i+1]=key;
    child[i+2]=right_page_id;

    parent_page.set_key_count(count+1);

    pager->write_page(parent_id,parent);
    
    if (count+1>=BtreePage::ORDER)
        split_internal(parent_id);
}

bool DiskBtree :: remove(int key){
    
    uint32_t curr_id=root_page;
    char BUFFER[PAGE_SIZE]={0};

    while (true){

        pager->read_page(curr_id,BUFFER);
        BtreePage node(BUFFER);

        if (node.is_leaf()){
            break;
        }

        int i=node.get_key_count()-1;
        int* keys=node.get_keys();

        while (i>=0 && key<keys[i]){
            i--;
        }

        curr_id=node.get_children()[i+1];
    }

    pager->read_page(curr_id,BUFFER);
    BtreePage node(BUFFER);

    int* keys=node.get_keys();
    auto* vals=node.values();
    int count=node.get_key_count();

    int i=0;
    while (i<count){
        if (keys[i]==key){
            while (i<count-1){
                keys[i]=keys[i+1];
                vals[i]=vals[i+1];
                i++;
            }
        }
        i++;
    }

    node.set_key_count(count-1);
    pager->write_page(curr_id,BUFFER);

    return true;
}