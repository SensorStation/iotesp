#pragma once

extern adc_oneshot_unit_handle_t adc_handle;
extern adc_cali_handle_t adc_cali_handle;
extern bool do_calibration;

#ifdef __cplusplus
extern "C" {
#endif

void adc_init();    

#ifdef __cplusplus
};
#endif 

