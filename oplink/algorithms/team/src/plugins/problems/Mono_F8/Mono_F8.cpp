/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo Gonz�lez
 *   - Carlos Segura Gonz�lez
 * 
 * FECHA
 *    Febrero 2011
 ************************************************************************************/

#include "Mono_F8.h"

double f8[1000] = {-7.704335e+00,5.358916e+00,-2.579697e+01,-3.090854e+01,4.526228e+00,2.665350e+01,1.811930e+01,2.255997e+01,-1.915169e+01,7.011923e+00,-2.882010e+01,2.786632e+01,-1.842369e+01,2.381080e+01,9.045241e+00,1.124680e+01,-1.806964e+01,-4.927334e+00,-1.607047e+01,-7.251332e+00,5.803853e+00,6.866449e+00,1.709345e+01,1.202418e+01,2.393866e+00,-2.116542e+00,2.948679e+01,-7.854470e+00,1.678328e+00,-7.845103e-01,1.762327e+01,4.068295e+00,2.561430e+01,-1.050809e+00,4.477378e+00,8.188890e+00,-2.673220e+01,9.846736e+00,-1.534732e+01,-3.794287e+00,-1.874251e+01,4.515248e-01,-1.582022e+01,1.657950e+01,-2.284151e+01,1.521968e+01,-1.105373e+01,1.839868e+01,1.444633e+01,9.469562e+00,
-1.046233e+01,2.612628e+01,3.154798e+01,3.141425e+01,-1.032815e+01,1.232405e+00,3.105191e+00,-2.967197e+01,6.546370e+00,1.732587e+01,-3.133761e+01,2.677208e+01,2.848613e+01,3.076262e+01,-7.459817e+00,-2.688125e+01,-2.182256e+01,-1.870155e+01,-1.888924e+01,-1.330091e+01,-9.509090e+00,-1.777511e+01,9.304190e+00,1.425747e+01,9.769204e+00,-4.097336e+00,1.630424e+01,-6.496445e+00,-2.641248e+01,-7.833406e+00,-6.798866e+00,-1.342048e+01,-3.105590e+01,1.468314e+01,-1.770546e+01,1.178209e+01,-9.656446e+00,2.202101e+01,6.183985e+00,-2.269545e+00,1.638482e+01,2.863035e+01,-2.941634e+01,5.150968e+00,-2.987965e+01,3.121078e+01,-9.191701e+00,-8.832901e+00,2.072609e+01,1.870585e+01,
3.131790e+01,9.369263e+00,1.185415e+01,2.400046e+01,2.230748e+01,-2.918508e+01,-1.324417e+01,5.657034e+00,2.797451e+01,1.981645e+00,-2.572580e+01,-2.075221e+00,-1.330145e+01,-2.676984e+01,1.128561e+01,2.597641e+01,-9.584650e+00,-1.847913e+00,1.031538e+00,-9.622565e+00,6.810159e+00,-2.225298e+01,-2.839255e+01,3.091553e+01,3.093669e+01,-9.616111e+00,3.251023e+00,2.110075e+01,-2.277702e+00,4.202206e+00,-1.414283e+01,-1.306518e+01,-1.739435e+01,-2.921430e+01,4.351175e+00,-3.180119e+01,-3.184260e+01,4.940062e+00,-1.614684e+01,-8.893134e+00,-3.140932e+01,9.649724e+00,-1.213936e+01,-3.145763e+01,1.771004e+01,1.415251e+01,1.740260e+01,-1.604600e+01,-3.195617e+01,2.137427e+00,
-1.730758e+01,1.751804e+01,2.124927e+01,2.790692e+01,-2.604893e+01,1.936421e+01,3.209882e+00,1.099735e+01,2.234996e+01,-1.839617e+01,-1.935551e+01,-2.610584e+01,1.714553e+01,-7.836275e+00,-2.168345e+01,-2.633154e+01,2.817232e+01,7.251600e+00,-1.152179e+01,4.299995e+00,-2.095277e+01,-2.879608e+01,-2.482992e+01,4.556479e-01,-2.918186e+01,2.979602e+01,2.718278e+01,6.691844e+00,6.422587e+00,-1.195275e+01,2.686136e+01,-1.208307e+01,2.059630e+01,2.039292e+01,8.886412e+00,9.345331e+00,1.039157e+00,2.573772e+01,-1.642658e+01,1.078421e+01,9.089609e+00,1.262159e+01,-1.245478e+01,-1.589237e+01,2.917343e+01,1.926400e+01,1.325619e+01,-3.123561e+01,2.903361e+01,5.917597e+00,
2.192125e+01,1.727559e+01,9.198557e-01,8.552440e+00,-1.895234e+01,-5.260141e+00,-1.349165e+01,3.137042e+01,1.726653e+01,-2.328792e+00,-1.149051e+01,-1.610283e+01,-1.989430e+01,-3.146597e+01,2.443312e+01,-1.467660e+01,-1.747188e+01,-2.357569e+01,3.070055e+00,-2.252573e+01,-1.322096e+01,-1.161071e+01,2.784306e+00,-2.518092e+01,-2.111052e+01,1.774544e+01,-3.086785e+01,-1.945456e+01,2.108704e+01,-7.957368e+00,2.204754e+01,8.866872e+00,7.946971e+00,-2.907282e+00,-9.664155e+00,1.520166e+01,-8.041892e+00,2.883130e+01,3.059985e+01,4.318459e+00,1.057263e+00,6.611175e+00,-2.592747e+01,-1.126813e+01,2.828938e+01,1.405472e+01,1.603121e+01,-1.387142e+01,1.104674e+01,-4.625720e+00,
1.904762e+01,9.617142e-01,3.941599e-01,1.659716e+01,-1.225642e+01,1.138403e+01,1.314164e+01,1.522622e+01,2.129355e+01,1.385601e+01,-1.728303e+01,-5.733939e+00,-1.917289e+00,2.238008e+01,-1.209459e+00,8.892148e+00,1.722414e+01,-2.271683e+01,-1.908097e+01,2.340288e+01,-3.681171e+00,-2.484399e+01,-3.720520e+00,9.279630e+00,5.874035e+00,-1.108394e+01,-2.654504e+01,-9.065498e+00,-2.006218e+01,1.946711e+01,1.422341e+01,2.542938e+01,-1.161080e+01,1.850781e+00,1.316171e+01,-9.930095e+00,1.066455e+01,2.032467e+00,1.779044e+01,-2.973570e+01,2.166606e+01,5.086164e+00,-8.544552e+00,-2.260309e+01,1.189493e+01,-6.987094e+00,-3.033839e+01,2.035214e+01,2.436275e+01,-1.358603e+01,
3.724463e+00,2.471268e+01,-2.415454e+01,2.793964e+01,-8.240697e+00,1.251573e+01,5.456705e+00,3.110287e+01,2.991631e+01,-2.707585e+01,2.884394e+01,-9.988446e+00,-2.925706e+01,1.615769e+01,-1.108770e+01,3.070140e+01,-3.060800e+01,1.054740e+01,1.638114e+01,-1.388084e+01,1.042567e+01,-3.757179e+00,1.388657e+01,-2.272364e+01,-4.181680e+00,2.883695e+01,-4.804762e-01,-2.391755e+01,2.096352e+01,-8.014823e+00,-1.045023e+01,1.560824e+01,-7.925638e+00,-3.049659e+01,1.673698e+01,-3.078243e+01,2.560672e+00,1.369162e+01,1.279978e+01,-2.149442e+01,1.846663e+01,-1.744208e+00,-2.273310e+00,2.060688e+01,-1.073329e+01,-1.281797e+01,2.830247e+01,8.996481e+00,6.582761e+00,2.895616e+01,
1.525320e+01,-1.498502e+01,1.656426e+01,1.492882e+01,3.071709e+01,-4.518878e+00,1.299813e+01,2.726004e+01,2.006827e+01,1.382007e+01,2.462995e+01,-1.461072e+01,-1.699799e+01,-1.896946e+01,-2.599604e+01,-1.635730e+01,-6.825397e+00,-6.073109e+00,-1.076413e+01,3.138001e+01,-6.702959e+00,1.241669e+01,2.603620e+01,9.553817e+00,-1.473315e+01,-5.975499e+00,-1.728276e+01,1.099233e+01,-1.329975e+01,2.892264e+01,-2.451007e-01,-9.087368e+00,-8.537516e-02,-2.785834e+01,-2.785628e+01,6.469824e+00,1.603677e+01,8.175714e+00,2.169493e+01,-5.740975e-02,-1.459960e+01,2.025838e+01,2.636739e+01,9.898589e-01,-1.936206e+01,2.105038e+01,-1.607908e+01,-3.117574e+01,2.229520e+01,1.218346e+01,
2.934983e+01,-1.663937e+01,-1.148688e+00,-1.586719e+01,3.065874e+01,-3.069235e+01,5.358020e+00,-3.850756e+00,2.788967e+00,-2.723638e+01,-1.244949e+01,3.061894e+01,3.051864e+01,1.059087e+01,2.168032e+01,-1.379434e+01,-1.927368e+01,9.027852e+00,6.681357e+00,-1.718577e+01,-4.370178e+00,1.977472e+01,3.193448e+01,-6.523424e+00,3.052707e+01,-1.535072e+01,2.106535e+01,2.557038e+01,9.295137e+00,9.566724e+00,-7.590860e+00,1.354955e+01,2.002220e+01,-5.351208e+00,-1.358209e+01,1.684508e+01,-2.415346e+01,-8.712524e+00,-2.796025e+01,-1.269598e+01,7.658174e+00,3.073842e+01,-2.246102e+01,-5.981505e+00,6.475919e+00,-6.330624e+00,-5.795517e+00,-1.559865e+01,-2.835652e+01,-3.030684e+01,
-5.785209e+00,-1.252182e+01,-2.087990e+01,1.929483e+01,8.974431e+00,-1.959053e+01,-7.097880e+00,9.391671e+00,2.427572e+01,3.851025e+00,1.499551e+01,2.571890e+01,2.287180e+01,-9.068993e+00,7.030656e+00,-1.972301e+01,-9.839924e+00,2.919414e+01,2.546461e+01,-2.070440e+01,6.081266e+00,1.357626e+01,2.959793e+01,2.699123e+01,-2.516954e+01,-1.450047e+01,-2.437028e+01,2.899695e+01,1.657824e+01,2.078784e+01,3.165545e+01,3.177502e+01,-6.873081e+00,2.956042e+00,-1.401349e+01,-1.244277e+01,1.283258e+01,1.749617e+01,-1.607773e+01,2.871182e+01,-2.581445e+01,3.406491e-01,-2.760912e+00,-1.520561e+01,-1.153578e+01,-2.889754e+01,-1.128364e+01,1.463214e+01,-2.026645e+01,2.422060e+01,
2.585738e+01,-2.859539e+01,-3.012408e+01,-1.596444e+01,1.062833e+01,4.436954e+00,-2.422320e+01,-3.659838e+00,-4.728798e+00,2.976071e+01,1.018026e+01,-1.856710e+01,-6.596475e+00,-1.150055e+01,-2.706697e+01,-2.490620e+01,-2.703856e+01,-2.907905e+01,1.024457e+00,9.812228e+00,2.392427e+01,-2.684621e+01,2.206117e+01,2.640360e+01,2.550723e+00,1.577370e+01,-6.242157e+00,-1.898018e+00,-2.948401e+01,-2.866002e+01,3.065157e+01,8.484185e-01,-4.559751e+00,-2.204382e+00,2.343649e+01,-5.155719e+00,-1.550364e+01,1.792175e+01,1.487343e+01,-9.887430e+00,-1.824667e+01,2.207954e+01,3.199014e+01,8.952964e-01,3.478959e+00,9.697946e+00,2.024987e+01,-2.828329e+01,2.784534e+01,-1.578329e+01,
5.231413e-01,1.569886e+01,8.808610e+00,2.947527e+00,-5.461725e+00,5.887928e+00,-2.113947e+01,-2.516909e+01,3.543585e+00,-1.383745e+01,2.439852e+01,-2.820827e+01,2.158602e+01,-2.775670e+01,-2.347467e+01,5.061784e+00,3.172895e+01,1.787568e+01,1.524316e+01,3.686280e+00,-2.529386e+01,1.709229e+01,8.702396e+00,-1.187432e+01,-2.396174e+01,1.018008e+01,2.356260e+01,8.076401e+00,3.586160e+00,-3.041180e+01,-4.437044e+00,-2.216841e+00,2.531958e+01,2.929533e+01,-2.817152e+01,-2.957803e+01,-4.229633e+00,1.261513e+01,-6.347475e+00,-1.822905e+00,2.144360e+01,2.592962e+01,-2.296843e+01,3.380991e+00,-4.850340e+00,-8.609671e-01,1.739937e+01,2.785995e+01,2.803187e+01,-3.085808e+01,
5.986972e+00,-7.847837e+00,-2.713904e+01,-2.118707e+01,1.337889e+01,-1.420109e+01,-2.514874e+01,-3.173182e+01,1.359070e+01,-3.099513e+01,-1.791395e+01,3.006331e+01,2.264974e+00,2.216281e+01,4.579470e+00,-9.149752e+00,5.097995e+00,-4.854060e-01,-2.410407e+01,2.696273e+01,-2.405294e-01,1.845542e+01,2.326726e+01,-1.267967e+01,2.990053e+01,-2.166149e+01,2.697053e+01,1.225015e+01,-5.938167e-02,1.122690e+01,2.786211e+01,5.388405e+00,2.612153e+01,-3.194362e+01,-1.228035e+01,-2.459150e+01,1.393757e+01,-2.543942e+01,2.723127e+01,2.206475e+01,1.765850e+01,5.927636e+00,2.306918e+01,-5.667028e-01,1.553178e+01,-1.718990e+01,-2.610817e+01,-3.613543e-01,-6.982702e+00,-1.981210e+01,
1.878787e+01,2.147138e+01,-2.068230e+00,4.524256e+00,3.187344e+01,2.246003e+01,3.044389e+01,5.340631e+00,4.422344e+00,1.491591e+01,1.467480e+01,3.831799e-02,-2.493031e+01,2.300289e+00,-1.917356e+01,1.590340e+01,-2.084673e+01,-2.233065e+00,2.204494e+01,1.052454e+01,-7.864330e+00,6.018254e+00,-2.256190e+00,-4.169490e+00,3.159181e+01,4.664083e+00,1.157665e+01,2.908048e+01,3.169023e+01,-2.946313e+01,-1.031767e+01,-1.072039e+01,-2.875691e+01,-2.692275e+01,1.045991e+01,-2.851257e+01,6.377861e+00,-1.422736e+01,7.698957e+00,2.179621e+01,-1.799856e+01,-1.984787e+01,-1.734030e+01,-1.511167e+01,2.443464e+01,1.934180e+01,-3.145333e+01,1.898031e+01,1.494218e+01,-1.277441e+01,
-1.137166e+01,-1.256413e+01,-2.789455e+01,9.391402e+00,-9.806491e+00,2.498292e+01,2.547743e+01,-2.496912e+01,-5.345471e+00,-3.037608e+00,-2.075038e+01,-3.013304e+01,1.196019e+01,2.479407e+01,2.775204e+01,1.215146e+01,-2.726067e+01,8.229628e-01,2.019421e+01,-2.395896e+01,2.505822e+01,-8.550827e+00,-2.464788e+01,-1.600392e-01,1.739175e+01,1.535843e+01,1.343966e+01,3.158254e+00,-2.340342e+01,-1.429351e+01,-2.807185e+01,2.045549e+01,-8.423548e+00,-4.618460e+00,1.245854e+01,-2.202522e+01,2.781299e+01,1.309538e+01,9.578914e+00,-2.895365e+01,-3.090827e+01,-1.672721e+01,1.418155e+01,1.891102e+01,-2.881468e+00,2.490512e+01,5.819807e+00,2.251112e+01,1.666223e+01,-1.700902e+01,
-1.582622e+01,1.059055e+00,5.041527e+00,2.130346e+00,-2.438893e+01,8.708042e+00,2.222242e+01,-2.319448e+01,-1.241413e-02,-2.728944e+01,-2.038889e+01,2.479147e+01,-4.001697e+00,-1.242386e+01,1.159399e-01,4.887806e+00,-9.104488e+00,6.417254e-01,-2.969689e+01,-1.115833e+01,2.156559e+01,-5.776784e+00,-1.791368e+01,2.643802e+01,6.446161e+00,1.971772e+01,2.830086e+01,1.499542e+01,2.161937e+01,9.335651e+00,-2.109851e+01,-2.224429e+01,-6.838304e+00,2.840922e+01,2.451397e+01,-6.581686e+00,-2.728487e+01,2.268201e+00,2.151064e+01,-1.144059e+01,-2.185958e+01,1.162043e+00,8.117901e+00,-2.861735e+01,2.789321e+01,-3.112420e+01,-8.207264e+00,9.353577e+00,-3.031508e+01,-9.330004e+00,
-2.852530e+01,1.451795e+01,3.680723e+00,-4.694917e+00,2.038978e+01,3.647021e+00,3.017463e+01,2.212929e+01,3.181186e+01,2.300975e+01,-2.843083e+01,-1.687564e+01,1.328720e+01,2.586912e+01,2.806714e+00,1.720603e+01,1.614648e+01,3.040427e+01,-2.983752e+01,1.691114e+01,9.374193e+00,1.033241e+00,-8.080076e+00,1.969217e+01,-9.151545e+00,6.607320e+00,1.200097e+01,1.381998e+01,-2.231182e+00,1.171746e+01,1.923236e+01,5.729458e+00,1.920592e+01,-2.852162e+01,1.090736e+01,-2.527773e+01,-2.450402e+01,-1.093873e+01,-2.877878e+01,1.195481e+01,-2.647880e+01,1.097987e+01,-1.452449e+01,2.856689e+01,2.239084e+01,-8.529225e+00,-2.864505e+01,-1.892537e+01,1.024955e+01,1.769175e+01,
-2.767773e+01,1.189852e+01,1.365559e+01,1.292481e+01,-2.532684e+01,-1.892689e+01,2.928099e+01,-5.779293e+00,-1.976536e+00,-2.718009e+01,2.278764e+01,1.239665e+00,-1.008381e+01,2.495917e+01,2.808507e+00,-9.635069e-01,4.031455e+00,-1.715619e+01,-3.102112e+01,1.553599e+01,-1.332807e+01,2.516093e+01,3.628019e+00,1.032098e+01,-2.064094e+01,7.886738e+00,1.614128e+01,1.252214e+00,2.589117e+01,9.055280e+00,-5.850372e+00,1.211543e+01,-2.365242e+01,-5.042244e+00,-1.896122e+01,2.795819e+01,-2.696192e+01,-2.833689e+01,1.624580e+01,-2.213780e+01,2.851530e+00,9.398035e+00,-2.506037e+01,-1.955020e+01,-1.386170e-01,9.736399e+00,-9.254443e+00,1.271933e+00,2.669464e+01,2.321411e+01,
-6.667374e+00,-1.486877e+01,-5.098174e+00,2.822817e+01,-2.417650e+01,-1.660594e+01,-2.959865e+01,-2.207519e+00,1.920215e+01,2.619001e+01,-6.080235e-01,1.924051e+01,1.724144e+01,-2.594683e+01,2.198354e+01,-1.200169e+01,1.343729e+00,-1.343348e+01,2.804872e+01,1.402631e+01,-7.675025e+00,3.103331e+01,1.621935e+01,-4.678693e+00,-1.593118e+01,1.983792e+01,5.027633e+00,-1.423143e+00,2.552717e+01,3.040221e+01,1.537941e+01,-4.398950e+00,1.404020e+01,-2.016776e+01,-7.330567e+00,-1.550722e+01,2.444199e+01,-2.459472e+01,5.464055e+00,2.224402e+01,-1.957171e+01,1.740802e+00,5.439585e+00,2.281166e+01,-1.703555e+01,2.463398e+01,4.146275e+00,2.406670e+01,-1.716525e+01,4.996621e+00,
4.869970e+00,-3.055494e+01,-1.506802e+01,2.291850e+01,-2.209702e+01,2.971186e+01,-1.046099e+01,-4.413201e+00,4.892916e+00,-1.352338e+01,-3.063579e+01,4.586372e+00,2.081554e+01,-2.079385e+01,-2.088043e+01,7.863164e+00,1.597026e+01,-2.709431e+01,2.174355e+00,3.098634e+01,-8.936516e+00,1.265376e+01,-6.029548e+00,-1.093568e+01,1.781114e+01,1.000664e+01,2.766554e+01,2.723620e+01,-4.954314e+00,-7.800511e+00,1.635837e+01,-1.756577e+00,1.178603e+01,1.937980e+01,1.979391e+01,2.528310e+01,2.407745e+01,1.944604e+01,-2.521534e+01,-2.594128e+01,-9.518860e+00,2.194249e+01,-2.923815e+01,-8.456712e+00,-1.426998e+00,3.085136e+01,-1.257363e+01,-1.947136e+00,-1.807099e+01,-3.042336e+01};

bool Mono_F8::init (const vector<string> &params){

  if (params.size() != 1) {
    cerr << "Error in init: Number of variables must be indicated" << endl;
    exit(-1);
  }
	
  setNumberOfVar(atoi(params[0].c_str()));
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void Mono_F8::evaluate (void) {
	long double Sum=0.0, Val=0.0;

	for (int i = 0; i < getNumberOfVar(); i++){  
		Val += (getVar(i) - f8[i]);
		Sum += Val * Val;
	}
  if (isinf(Sum)) {
	  setObj(0, LDBL_MAX);
  }
  else { 
	  setObj(0, Sum);
  }
}

// Clonacion
Individual* Mono_F8::clone (void) const{
	return new Mono_F8();
}