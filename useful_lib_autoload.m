function useful_lib_autoload()
  % Configure library
  dir_path = fileparts(which(mfilename('fullpath')));

  setenv('LIB_USEFUL_LIB_ROOT_DIRECTORY', dir_path);
  addpath(genpath(fullfile(dir_path, 'matlab')));
  addpath(genpath(fullfile(dir_path, 'c', 'quantization', 'interfaces', 'matlab'))); % Needed to access interface compilers
  addpath(genpath(fullfile(dir_path, 'c', 'utils', 'interfaces', 'matlab'))); % Needed to access interface compilers

  % Compile MEX files
  fprintf('>> Generate MEX files for the useful libraries\n');
  mex_nonuniform_quantization_compiler();
  mex_uniform_quantization_compiler();
  mex_uniform_quantization_data_compiler();
  
  mex_concatenate_compiler();
end