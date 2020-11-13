function mex_uniform_quantization_compiler()
    current_folder = fileparts(which(mfilename('fullpath')));
    
    mex_compiler( ...
        current_folder, ...
        'mex_uniform_quantization', ...
        { ...
            fullfile(current_folder, '..', '..', 'quantizer.c')
        }, ...
        {'-R2018a'} ...
    );
end