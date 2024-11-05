
#include <sycl/ext/intel/ac_types/ac_int.hpp>
#include <sycl/ext/intel/fpga_extensions.hpp>
#include <sycl/sycl.hpp>
#include <CL/sycl.hpp>
#include <dpct/dpct.hpp>

void CalcSplineWeightsFPGA() {
#if FPGA_SIMULATOR
    auto selector = sycl::ext::intel::fpga_simulator_selector_v;
#elif FPGA_HARDWARE
    auto selector = sycl::ext::intel::fpga_selector_v;
#else  // #if FPGA_EMULATOR
    auto selector = sycl::ext::intel::fpga_emulator_selector_v;
#endif

    // create the device queue
    sycl::queue queue(selector);

    auto device = queue.get_device();

  using PipeDataA = fpga_tools::NTuple<TT, tile_a>;
  using PipeDataB = fpga_tools::NTuple<TT, tile_b>;
  using PipeDataC = fpga_tools::NTuple<TT, tile_a>;

  // Allocate USM memory for coeff_many
  float* coeff_many_usm = sycl::malloc_shared<float>(/*size*/, queue);

  queue.submit([&](sycl::handler& cgh) {
    cgh.parallel_for(sycl::range<1>(NSplines_valid), [=](sycl::id<1> idx) {
      unsigned int splineNum = idx[0];

      const short int Param = cpu_spline_handler->paramNo_arr[splineNum];
      const short int segment = segments[Param];
      const short int segment_X = Param * _max_knots + segment;
      const unsigned int CurrentKnotPos = cpu_spline_handler->nKnots_arr[splineNum]  nCoeff + segment  _nCoeff_;

      const float fY = coeff_many_usm[CurrentKnotPos];
      const float fB = coeff_many_usm[CurrentKnotPos + 1];
      const float fC = coeff_many_usm[CurrentKnotPos + 2];
      const float fD = coeff_many_usm[CurrentKnotPos + 3];
      const float dx = vals[Param] - cpu_spline_handler->coeff_x[segment_X];

      cpu_weights_var[splineNum] = sycl::fma(dx, sycl::fma(dx, sycl::fma(dx, fD, fC), fB), fY);
    });
  }).wait();

  queue.submit([&](sycl::handler& cgh) {
    cgh.parallel_for(sycl::range<1>(NTF1_valid), [=](sycl::id<1> idx) {
      unsigned int tf1Num = idx[0];

      const float x = vals[cpu_paramNo_TF1_arr[tf1Num]];
      const float a = cpu_coeff_TF1_many[tf1Num * _nTF1Coeff_];
      const float b = cpu_coeff_TF1_many[tf1Num * nTF1Coeff + 1];

      cpu_weights_tf1_var[tf1Num] = sycl::fma(a, x, b);
    });
  }).wait();

  // Free USM memory
  sycl::free(coeff_many_usm, queue);
}