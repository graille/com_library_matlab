function mex_compiler(directory, mex_name, dependencies, arguments)
  % Clear all current used MEX files (needed to be able to replace them)
  clear mex
  
  if nargin < 4, arguments = {}; end

  % Move to the directory of the C file
  current_folder = pwd;
  cd(directory);

  try
    internal_dependencies = {
      fullfile(getenv('LIB_USEFUL_LIB_ROOT_DIRECTORY'), 'c', 'misc', 'pmath.c'), ...
      fullfile(getenv('LIB_USEFUL_LIB_ROOT_DIRECTORY'), 'c', 'misc', 'pstring.c'), ...
      fullfile(getenv('LIB_USEFUL_LIB_ROOT_DIRECTORY'), 'c', 'misc', 'ptime.c') ...
    };

    % Process
    for d=1:length(internal_dependencies)
        dependencies{end + 1} = internal_dependencies{d};
    end

    % Check matlab version
    % This will be used to determine the usage of version flag
    matlab_version = version();
    matlab_version_code = str2double(matlab_version(1:3));

    version_after_2018a = false;
    if(matlab_version_code >= 9.4)
      version_after_2018a = true;
    end

    % Create compilation chain
    compilation_chain = { ...
      '-silent', ... % Flag used to hide compiler messages
      '-O' ...  % Flag used to apply speed optimisation
    };

    % Add arguments to compilation chain
    for i=1:length(arguments)
      switch arguments{i}
        case {'-R2018a', '-R2017b', '-r2018a', '-r2017b', '-R2018A', '-R2017B', '-r2018A', '-r2017B'}
          if version_after_2018a
            compilation_chain{end + 1} = arguments{i};
          end
        otherwise
          compilation_chain{end + 1} = arguments{i};
      end
    end

    % Add sources to compilation chain
    compilation_chain{end + 1} = [mex_name '.c'];
    for i=1:length(dependencies)
      compilation_chain{end + 1} = dependencies{i};
    end
    
    % Compile C file into a MEX file
    try
      fprintf('\tCompile MEX file from %s.c\n', mex_name);
      mex(compilation_chain{:});
      
      if ~isempty(getenv('MEX_DIRECTORY'))
        % If we have specified a mex direcory, we move the file to it
        fprintf('\tMove MEX file to appropriate directory\n');
        movefile([mex_name '.mexw64'], getenv('MEX_DIRECTORY'));
      end
    catch e
      fprintf('Error when generating the MEX files from %s\n', mex_name);
      e
    end
  catch e
    fprintf('An internal error occured');
    e
  end
  
  % Return to previous folder
  cd(current_folder);
end
