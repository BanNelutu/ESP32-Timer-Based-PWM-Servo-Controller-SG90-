#include <stdio.h>
#include "driver/gptimer.h"
#include "driver/gpio.h"

#define GPIO_PIN_NUM GPIO_NUM_27

volatile int count=0;
volatile int alarmDegre=2400;

void changeDegre(){
    if(count<100){
        alarmDegre=2400;
    }
    else if(count<200&&count>100){
        alarmDegre=1500;
    }
    else if(count>200&&count<300)
        alarmDegre=550;
    else if(count >=300){
        count=0;}
}

void pulse(gptimer_handle_t timer,const gptimer_alarm_event_data_t *edata){
    changeDegre();
    if(count%2==0){
        gpio_set_level(GPIO_PIN_NUM,0);
        gptimer_alarm_config_t alarm_config={
        .reload_count=0,
        .alarm_count = 20000-alarmDegre,
        .flags.auto_reload_on_alarm=true,
        };
        gptimer_set_alarm_action(timer,&alarm_config);
        
        
    }
    else if(count%2==1){
        gpio_set_level(GPIO_PIN_NUM,1);
        gptimer_alarm_config_t alarm_config={
        .reload_count=0,
        .alarm_count =alarmDegre,
        .flags.auto_reload_on_alarm=true,
        };
        gptimer_set_alarm_action(timer,&alarm_config);
    }
    count++;
}

static bool timer_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){

    pulse(timer,edata);
    return false;

}

void app_main(void)
{
gpio_set_direction(GPIO_PIN_NUM, GPIO_MODE_OUTPUT); 
gptimer_handle_t timer = NULL;
gptimer_config_t config={
    .clk_src= GPTIMER_CLK_SRC_DEFAULT,
    .direction= GPTIMER_COUNT_UP,
    .resolution_hz=1*1000*1000,
};
ESP_ERROR_CHECK(gptimer_new_timer(&config,&timer));
gptimer_alarm_config_t alarm_congif={
    .reload_count=0,
    .alarm_count=alarmDegre,
    .flags.auto_reload_on_alarm=true,
    
};
ESP_ERROR_CHECK(gptimer_set_alarm_action(timer,&alarm_congif));
gptimer_event_callbacks_t cbs = {
    .on_alarm=timer_alarm_callback,
};

ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer,&cbs,NULL));
ESP_ERROR_CHECK(gptimer_enable(timer));
ESP_ERROR_CHECK(gptimer_start(timer));
gpio_set_level( GPIO_PIN_NUM,1);

    
}
