#include <moonlight.h>
/*
void * moonlight_cw(XLsoot_par *par){
    printf("connectwith!\n");
    return NULL;
}
/*

void * moonlight_dcw(XLsoot_par *par){
    printf("disconnectwith!\n");
    return NULL;
}

void * moonlight_c(XLsoot_par *par){
    printf("connect!\n");
    return NULL;
}

void * moonlight_dc(XLsoot_par *par){
    printf("disconnect!\n");
    return NULL;
}
*/

void *moonlight_event(XLevent_par * par){
    printf("moonlight event!\n\n");
  /*  soot_create(par->event_id,"connect",connect_soot);
    soot_create(par->event_id,"disconnect",disconnect_soot);
    soot_create(par->event_id,"connectwith",connectwith_soot);
    soot_create(par->event_id,"disconnectwith",disconnectwith_soot);
    */
    while(1){
       /* device_set_update_mode(par->event_id);
        char str[32]="";
        for(int i=0;i<4;i++){
            strcat(str,"1");
            XLdevice * device=device_get_update(par->event_id,str);
            if(device==NULL){
                device=device_create_and_get(par->event_id,str);
                strcpy(device->info.,str);
                strcpy(device->info.protocol,"MOONLIGHT");
                device->info.direction=DEVICE_DIRECTION_IN;
            }
        }
        device_del_not_update(par->event_id);*/
        sleep(1);

    }
    return NULL;
}

void moonlight_init(void){
    app_add("moonlight",moonlight_event);
    event_create_and_run("moonlight");
}
