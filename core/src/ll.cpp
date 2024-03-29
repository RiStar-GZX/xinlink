 #include <ll.h>

void data_sh(uint8_t *data,uint size){
    uint8_t *data_p=data;
    for(int i=0;i<size;i++){
        if(*data_p=='\0')printf("!");
        else printf("%X",*data_p);
        data_p++;
    }
    printf("\n");
}

XLll * ll_create(uint size){
    if(size<=0)return NULL;
    XLll * ll=(XLll*)malloc(sizeof(XLll));
    ll->member_size=size;
    ll->head=NULL;
    ll->tail=NULL;
    ll->member_num=0;
    return ll;
}

XLll_member * ll_member_create(XLll * ll,void * member){
    XLll_member *member_new=(XLll_member *)malloc(sizeof(XLll_member));
    member_new->data=malloc(ll->member_size);
    memcpy(member_new->data,member,ll->member_size);
    //member_new->size=member_size;
    return member_new;
}

int ll_add_member_tail(XLll * ll,void * member,uint member_size){
    if(ll==NULL||member==NULL)return -1;
    if(member_size<=0||member_size>ll->member_size)return -1;

    XLll_member *member_new=ll_member_create(ll,member);
    if(ll->head==NULL){
        ll->head=member_new;
        ll->tail=member_new;
        ll->member_num=1;
        member_new->front=NULL;
        member_new->next=NULL;
        return 1;
    }

    ll->tail->next=member_new;
    member_new->front=ll->tail;
    member_new->next=NULL;
    ll->member_num++;
    ll->tail=member_new;
    return 1;
}

int ll_add_member_head(XLll * ll,void * member,uint member_size){
    if(ll==NULL||member==NULL)return -1;
    if(member_size<=0||member_size>ll->member_size)return -1;

    XLll_member *member_new=ll_member_create(ll,member);
    if(ll->head==NULL){
        ll->head=member_new;
        ll->tail=member_new;
        ll->member_num=1;
        member_new->front=NULL;
        member_new->next=NULL;
        return 1;
    }

    ll->head->front=member_new;
    member_new->next=ll->head;
    member_new->front=NULL;
    ll->member_num++;
    ll->head=member_new;
    return 1;
}

int ll_insert_member_next(XLll * ll,void * member,uint member_size,uint member_num){
    if(ll==NULL||member==NULL)return -1;
    if(member_size<=0||member_size>ll->member_size)return -1;
    if(member_num<0||member_num>ll->member_num)return -1;

    XLll_member *member_new=ll_member_create(ll,member);
    XLll_member * member_now=ll->head;
    for(int i=0;i<member_num;i++){
        member_now=member_now->next;
    }

    member_new->next=member_now->next;
    if(member_new->next==NULL)ll->tail=member_new;
    member_new->front=member_now;
    member_now->next=member_new;
    ll->member_num++;
    return 1;
}

int ll_insert_member_front(XLll * ll,void * member,uint member_size,uint member_num){
    if(ll==NULL||member==NULL)return -1;
    if(member_size<=0||member_size>ll->member_size)return -1;
    if(member_num<0||member_num>ll->member_num)return -1;

    XLll_member *member_new=ll_member_create(ll,member);
    XLll_member * member_now=ll->head;
    for(int i=0;i<member_num;i++){
        member_now=member_now->next;
    }

    member_new->front=member_now->front;
    if(member_new->front==NULL)ll->head=member_new;
    member_new->next=member_now;
    member_now->front=member_new;
    ll->member_num++;
    return 1;
}

int ll_add_ll(XLll * ll1,XLll* ll2){
    if(ll1==NULL||ll2==NULL)return -1;
    if(ll2->member_size>ll1->member_size)return -1;
    XLll_member * member_now=ll2->head;
    for(int i=0;i<ll2->member_num;i++){
        ll_add_member_tail(ll1,member_now,ll2->member_size);
        member_now=member_now->next;
    }
    return 1;
}

int ll_del_member_head(XLll * ll){
    return ll_del_member_num(ll,0);
}

int ll_del_member_tail(XLll * ll){
    return ll_del_member_num(ll,ll->member_num-1);
}

int ll_del_member_num(XLll * ll,uint member_num){
    if(ll==NULL||member_num<0||member_num>ll->member_num)return -1;
    if(ll->head==NULL||ll->tail==NULL)return -1;

    XLll_member * member_now=ll->head;
    ll->member_num--;
    //只剩一个成员
    if(ll->head==ll->tail){
        ll->head=NULL;
        ll->tail=NULL;
        free(member_now);
        return 1;
    }
    //其他情况
    for(int i=0;i<member_num;i++){
        member_now=member_now->next;
    }
    if(member_now->front==NULL)ll->head=member_now->next;
    else member_now->front->next=member_now->next;
    if(member_now->next==NULL)ll->tail=member_now->front;
    else member_now->next->front=member_now->front;

    free(member_now);
    return 1;
}

int ll_del_member_all(XLll * ll){
    if(ll==NULL)return 0;
    for(int i=0;i<ll->member_num;i++)ll_del_member_num(ll,0);
    return 1;
}

void ll_free(XLll * ll){
    if(ll==NULL)return;
    ll_del_member_all(ll);
    free(ll);
}


void * ll_get_member(XLll * ll,uint member_num){
    if(ll==NULL||member_num<0||member_num>ll->member_num)return NULL;
    if(ll->head==NULL||ll->tail==NULL)return NULL;

    XLll_member * member_now=ll->head;
    for(int i=0;i<member_num;i++){
        return member_now->data;
        member_now=member_now->next;
    }
    return NULL;
}

XLll_member * ll_get_member_compare(XLll * ll,uint front_size,uint size,void *compare){
    if(ll==NULL&&compare==NULL)return NULL;
    if(ll->head==NULL||ll->tail==NULL)return NULL;
    if(ll->member_size<front_size+size)return NULL;

    XLll_member * member_now=ll->head;
    for(int i=0;i<ll->member_num;i++){
        uint8_t * data=(uint8_t*)member_now->data;
        data+=front_size;
        if(memcmp(data,compare,size)==0)return member_now;
        member_now=member_now->next;
    }
    return NULL;
}

int ll_del_member(XLll *ll,XLll_member * member){
    if(ll==NULL||member==NULL)return 0;
    XLll_member * mem=ll->head;
    for(int i=0;i<ll->member_num;i++){
        if(mem==member)return ll_del_member_num(ll,i);
        mem=mem->next;
    }
    return 0;
}

void ll_show_member(XLll *ll){
    if(ll==NULL)return;
    if(ll->head==NULL)printf("NO member!\n");
    XLll_member * member_now=ll->head;
    for(int i=0;i<ll->member_num;i++){
        printf("num:%d ",i);
        if(member_now->data==NULL)printf("without data!\n");
        else  data_sh((uint8_t*)member_now->data,ll->member_size);
        member_now=member_now->next;
    }
}

int ll_cpy(XLll * dest,XLll *src){
    if(dest==NULL||src==NULL)return 0;
    ll_del_member_all(dest);
    XLll_member * member=src->head;
    for(int i=0;i<src->member_num;i++){
        ll_add_member_tail(dest,member->data,src->member_size);
        member=member->next;
    }
    return 1;
}
