function [ber, EbN0dBs] = ber_evaluator(ilocal_transceiver, EbN0dBs, nb_errors, nb_workers, matfile)
    %{
      Input arguments:
        - transceiver: Function perofrming the simulation such as
            [b_emitted, b_received] = transceiver(EbN0dB)
          Return the bits emitted and received for a specified Eb/N0 (in dB)
        - EbN0dBs: List of EbN0 which will be simulated (in dB)
        - nb_errors: (optional) Reference of number of errors for each simulation (Default: 100)
            If nb_errors = -1, then the simulation will loop and run forever
        - nb_workers: (optional) Number of parallel workers for the parfor (Default: 1)
        - matfile: (optional) Name of the file used for intermediate results storage
      Output arguments:
        - ber: Vector contaning the procesed BER
        - EbN0dBs: The actual vector of Eb/N0 (in dB) used to compute the BER vector
    %}
  
    if nargin < 3, nb_errors = 100; end
    if nargin < 4, nb_workers = 1; end
    if nargin < 5, matfile = false; end
    
    % Infinite loop management
    loop_finished = false;
    loop_perpetual = false;
    loop_iter = 0;
    
    EbN0dBs = unique(EbN0dBs);
    EbN0dBs_initial = EbN0dBs;
    
    ber_total_points = zeros(1, length(EbN0dBs));
    ber_total_errors = zeros(1, length(EbN0dBs));
    
    % Matfile management
    if matfile ~= false
      fprintf('Working on matfile %s\n', matfile);
      
      if isfile(matfile)
        fprintf('Load existing MatFile %s... ', matfile);
        
        mFile = load(matfile, 'EbN0dBs', 'ber_total_points', 'ber_total_errors');
        EbN0dBs = unique([EbN0dBs mFile.EbN0dBs]);
        ber_total_points = zeros(1, length(EbN0dBs));
        ber_total_errors = zeros(1, length(EbN0dBs));
        
        % Load already calculated results from Matfile
        for i=1:length(EbN0dBs)
          [v, p] = ismember(EbN0dBs(i), mFile.EbN0dBs);
          if v
            ber_total_points(i) = mFile.ber_total_points(p);
            ber_total_errors(i) = mFile.ber_total_errors(p);
          end
        end
        
        fprintf('Matfile loaded.\n');
      else
        save(matfile, 'EbN0dBs', 'ber_total_errors', 'ber_total_points', '-v7');
      end
      
      if nb_errors == -1
        loop_perpetual = true;
        nb_errors = 100;
      end  
    else
      if nb_errors == -1
        error('You cannot use the infinite loop calculation without specify a Matfile');
      end  
    end
    
    ber = zeros(1, length(EbN0dBs));
    
    while loop_finished ~= true
      loop_iter = loop_iter + 1;
      for i=1:length(EbN0dBs)
        if ismember(EbN0dBs(i), EbN0dBs_initial)
          required_nb_errors = ceil(1/10^(EbN0dBs(i)/10)*nb_errors) * loop_iter;
          fprintf('\tStart Eb/N0 = %d dB (iteration %d) [require %d errors] -- Started %s\n', EbN0dBs(i), loop_iter, required_nb_errors, datetime(now, 'ConvertFrom', 'datenum'));
          
          nchar = 0;
          while ber_total_errors(i) < required_nb_errors
            % Execute workers loop
            worker_total_points = zeros(1, nb_workers);
            worker_nb_errors = zeros(1, nb_workers);
            
            parfor j=1:nb_workers
              [b_emitted, b_received] = ilocal_transceiver(EbN0dBs(i));
              
              worker_nb_errors(j) = sum(xor(b_emitted, b_received));
              worker_total_points(j) = length(b_emitted);
            end
            
            ber_total_errors(i) = ber_total_errors(i) + sum(worker_nb_errors);
            ber_total_points(i) = ber_total_points(i) + sum(worker_total_points);
            
            if matfile ~= false
              save(matfile, 'EbN0dBs', 'ber_total_errors', 'ber_total_points', '-v7');
            end
            
            % Logging
            fprintf(repmat('\b', 1, nchar));
            nchar = fprintf('\t\t EbN0 = %d dB || (%d/%d) [require %d]\n', EbN0dBs(i), ber_total_errors(i), ber_total_points(i), required_nb_errors);
          end
        end
        
        ber(i) = ber_total_errors(i) / ber_total_points(i);
      end
      
      if ~loop_perpetual, loop_finished = true; end
    end
  end