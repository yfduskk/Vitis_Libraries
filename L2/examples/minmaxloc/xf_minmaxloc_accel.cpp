/***************************************************************************
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CXFSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#include "xf_min_max_loc_config.h"

extern "C" {

void minmaxloc_accel(
    ap_uint<PTR_WIDTH>* img_in, int32_t* min_max_value, uint16_t* min_max_loc_xy, int height, int width) {
    // clang-format off
    #pragma HLS INTERFACE m_axi      port=img_in          offset=slave  bundle=gmem0
    #pragma HLS INTERFACE m_axi      port=min_max_value   offset=slave  bundle=gmem1
    #pragma HLS INTERFACE m_axi      port=min_max_loc_xy  offset=slave  bundle=gmem2
    #pragma HLS INTERFACE s_axilite  port=height 			bundle=control
    #pragma HLS INTERFACE s_axilite  port=width 			bundle=control
    #pragma HLS INTERFACE s_axilite  port=return 			bundle=control
    // clang-format on

    // Local objects:
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1> imgInput(height, width);
    int32_t min_value, max_value;
    uint16_t _min_locx, _min_locy, _max_locx, _max_locy;

    // clang-format off
    #pragma HLS STREAM variable=imgInput.data depth=2
    // clang-format on

    // clang-format off
    #pragma HLS DATAFLOW
    // clang-format on

    // Retrieve xf::cv::Mat objects from img_in data:
    xf::cv::Array2xfMat<PTR_WIDTH, TYPE, HEIGHT, WIDTH, NPC1>(img_in, imgInput);

    // Run xfOpenCV kernel:
    xf::cv::minMaxLoc<TYPE, HEIGHT, WIDTH, NPC1>(imgInput, &min_value, &max_value, &_min_locx, &_min_locy, &_max_locx,
                                                 &_max_locy);

    // Copy local outputs to global pointer:
    min_max_value[0] = min_value;
    min_max_value[1] = max_value;
    min_max_loc_xy[0] = _min_locx;
    min_max_loc_xy[1] = _min_locy;
    min_max_loc_xy[2] = _max_locx;
    min_max_loc_xy[3] = _max_locy;

    return;
} // End of kernel

} // End of extern C
