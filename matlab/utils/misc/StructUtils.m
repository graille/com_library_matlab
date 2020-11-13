classdef StructUtils
  methods(Static)
    function r = has_option(options, option_name)
      % Source: https://stackoverflow.com/questions/28179401
      names   = fieldnames(options);
      is_field = strcmpi(option_name, names);
      
      r = false;
      if any(is_field)
        r = true;
      end
    end
    
    function r = option_value(options, option_name)
      % Source: https://stackoverflow.com/questions/28179401
      names = fieldnames(options);
      is_field = strcmpi(option_name, names);
      
      r = [];
      if any(is_field)
        r = options.(names{is_field});
      end
    end
    
    function r = get_option_value(options, option_name, default_value)
      if StructUtils.has_option(options, option_name)
        r = StructUtils.option_value(options, option_name);
      else
        r = default_value;
      end
    end
  end
end