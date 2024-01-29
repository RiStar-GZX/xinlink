#include  <xlplugin.h>
#include <type.h>

/*
XLll plu_ll=*ll_create(sizeof(PLU_info));

uint plu_add(XLll * pos_ll){
    if(pos_ll==NULL)return 0;
    extern XLll plu_ll;
    XLplu plu;
    plu.ll=pos_ll;
    ll_add_member_tail(&plu_ll,&plu,sizeof(XLplu));
}

uint plu_del(XLll * pos_ll){
    extern XLll plu_ll;
    XLll_member *member=plu_ll.head;
    for(int i=0;i<plu_ll.member_num;i++){
        XLplu * plu=(XLplu*)member->data;
        if(plu->ll==pos_ll)ll_del_member(&plu_ll,i);
        member=member->next;
    }
    return 1;
}
*/
/*
int send_pluinfo(XLll *pos){
    return ll_add_member_tail(pos,,sizeof(PLU_info));
}*/

int send_event_info(){

}
