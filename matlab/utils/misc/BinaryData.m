classdef BinaryData
  methods(Static)
    function output_size = generate_binary(output_filepath, elements, params)
      if nargin < 3, params = struct(); end
      params_names = fieldnames(params);

      output_file = fopen(output_filepath, 'w+');
      output_endianness = 'ieee-le';

      output_size = 0;

      %% Write properties

      % Endianness
      %   Write on 8 bits
      %     -> Value 0x11 = little endian
      %     -> Value 0x22 = big-endian

      if(strcmpi(output_endianness, 'ieee-le') || strcmpi(output_endianness, 'le') || strcmpi(output_endianness, 'l'))
        fwrite(output_file, hex2dec('11'), 'uint8', 0, output_endianness);
      elseif (strcmpi(output_endianness, 'ieee-be') || strcmpi(output_endianness, 'be') || strcmpi(output_endianness, 'b'))
        fwrite(output_file, hex2dec('22'), 'uint8', 0, output_endianness);
      else
        error(sprintf('Unknwon endianness type %s', output_endianness));
      end

      output_size = output_size + 8;
      
      %% Write params
      params_name = fieldnames(params);
      fwrite(output_file, 0, 'uint32', 0, output_endianness);

      %{
      fwrite(output_file, length(params_name), 'uint32', 0, output_endianness);
      output_size = output_size + 32;

      for i=1:length(params_name)
        % Write param name
        for k=1:length(params_name{i})
          fwrite(output_file, params_name{i}(k), 'char*1', 0, output_endianness);
        end

        for k=1:1024-length(params_name{i})
          fwrite(output_file, 0, 'char*1', 0, output_endianness);
        end

        % Write param type

      end
      %}
      
      %% Write elements
      %   Elements type:
      %     -> Double: 0x11
      %     -> int32: 0x22
      %     -> int16: 0x23
      %   See read_binary.h for more information

      fwrite(output_file, hex2dec('11'), 'uint8', 0, output_endianness); % Elements type
      fwrite(output_file, length(elements), 'uint32', 0, output_endianness); % Number of elements

      output_size = output_size + 32;
      
      % Elements
      for elt=elements
        fwrite(output_file, elt, 'double', 0, output_endianness);
        output_size = output_size + 64;
      end

      output_size = output_size / 8;
      
      fclose(output_file);
    end
  end
end