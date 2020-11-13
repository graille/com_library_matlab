function [ber, EbN0dBs] = ber_values_finder(ilocal_transceiver, Ts, SdB, incrementdB, epsilon, nb_workers, matfile)
  %{ 
    Giving a specific set of BER, this function return the values of Eb/N0 needed to evaluate a transceiver at the specified BER.
    Input arguments:
      - local_transceiver: Function perofrming the simulation such as
          [b_emitted, b_received] = transceiver(EbN0dB)
        Return the bits emitted and received for a specified Eb/N0 (in dB)
      - Ts: Set of BER values we want to reach
      - SdB: Starting value of research in dB (default: -8dB)
      - incrementdB: Initial increment in dB (default: 1dB)
      - epsilon: Interval of validation

      - nb_workers: (optional) Number of parallel workers for the parfor (Default: 1)
      - matfile: (optional) Name of the file used for intermediate results storage
    Output arguments:
      - ber: Vector contaning the procesed BER
      - EbN0dBs: The actual vector of Eb/N0 (in dB) used to compute the BER vector
  %}

  %% Inputs management
  if nargin < 2, Ts = 1e-5; end
  if nargin < 3, SdB = -8; end
  if nargin < 4, incrementdB = 1; end
  if nargin < 5, epsilon = 0.1; end
  if nargin < 6, nb_workers = 1; end
  if nargin < 7, matfile = false; end
  
  Ts = sort(Ts, 'descend');

  EbN0dBs = [];
  ber_total_points = [];
  ber_total_errors = [];
  ber = [];

  %% Matfile management
  if matfile ~= false
    fprintf('Working on matfile %s\n', matfile);
    
    if isfile(matfile)
      fprintf('Load existing MatFile %s... ', matfile);
      
      mFile = load(matfile, 'EbN0dBs', 'ber_total_points', 'ber_total_errors');

      % Load already calculated results from Matfile
      EbN0dBs = mFile.EbN0dBs;
      for i=1:length(EbN0dBs)
        [~, p] = ismember(EbN0dBs(i), mFile.EbN0dBs);
        ber_total_points(i) = mFile.ber_total_points(p);
        ber_total_errors(i) = mFile.ber_total_errors(p);
      end
      
      fprintf('Matfile loaded.\n');
    else
      save(matfile, 'EbN0dBs', 'ber_total_errors', 'ber_total_points', '-v7');
    end
  end

  %% Useful functions
  function add_point(EbN0dB, nb_points, nb_errors)
    [loa, lob] = ismember(EbN0dB, EbN0dBs);

    if ~loa
      EbN0dBs(end + 1) = EbN0dB;
      ber_total_points(end + 1) = nb_points;
      ber_total_errors(end + 1) = nb_errors;
    else
      ber_total_points(lob) = ber_total_points(lob) + nb_points;
      ber_total_errors(lob) = ber_total_errors(lob) + nb_errors;
    end

    % Sort array
    [EbN0dBs, I] = sort(EbN0dBs, 'ascend');
    ber_total_points = ber_total_points(I);
    ber_total_errors = ber_total_errors(I);
    
    ber = ber_total_errors ./ ber_total_points;

    if matfile ~= false
      save(matfile, 'EbN0dBs', 'ber_total_errors', 'ber_total_points', '-v7');
    end
  end

  %% Eb/N0 finder loop
  current_EbN0dB = SdB;
  last_non_zero_EbN0dB = current_EbN0dB;
  
  for ii=1:length(Ts)
    % Initialization
    visited_EbN0dB = [];
    visited_EbN0dB_ber = [];
    current_increment = incrementdB;
    
    T = Ts(ii);
    
    % Check in file if a value already exist for this treashold
    skip_value = false;
    for jj=1:length(EbN0dBs)
      local_ber = ber_total_errors(jj) / ber_total_points(jj);
      
      if (local_ber >= T * 10^(-epsilon)) && (local_ber <= T * 10^(epsilon))
        fprintf('Eb/N0 value for BER = 10^(%4.2f) already in file with Eb/N0 = %4.4f dB BER = 10^(%4.4f)] \n', log10(T), EbN0dBs(jj), log10(local_ber));
        skip_value = true;
        break;
      end
    end
    if skip_value, continue; end
    
    % If the value is not reached currently, we try to reach it
    tested_points = ceil(10 / T);
    
    fprintf('Find Eb/N0 value for BER = 10^(%4.2f). \n', log10(T));
    while true
      visited_EbN0dB(end + 1) = current_EbN0dB;
      
      fprintf('\t Currently tested: Eb/N0 = %4.5f dB... ', current_EbN0dB);
      worker_total_points = zeros(1, nb_workers);
      worker_nb_errors = zeros(1, nb_workers);
      
      nchar = 0;
      while sum(worker_total_points) < tested_points
        parfor j=1:nb_workers
          [b_emitted, b_received] = ilocal_transceiver(current_EbN0dB);

          worker_nb_errors(j) = worker_nb_errors(j) + biterr(b_emitted, b_received);
          worker_total_points(j) = worker_total_points(j) + length(b_emitted);
        end
        
        local_ber = sum(worker_nb_errors) / sum(worker_total_points);

        % Logging
        fprintf(repmat('\b', 1, nchar));
        nchar = fprintf('%d / %d points tested [%d errors - BER = 10^(%4.4f)]. ', sum(worker_total_points), tested_points, sum(worker_nb_errors), log10(local_ber));
      end

      % Determine next move
      visited_EbN0dB_ber(end + 1) = local_ber;
      
      if local_ber ~= 0
        last_non_zero_EbN0dB = current_EbN0dB;
      end
      
      if (local_ber < T * 10^(-epsilon)) || (local_ber > T * 10^(epsilon))
        % Calculate the new evaluation point 
        if length(visited_EbN0dB_ber) < 2
          fprintf('I\n');
          new_value = 10^((current_EbN0dB + incrementdB) / 10);
        else
          if visited_EbN0dB_ber(end) == 0 && visited_EbN0dB_ber(end - 1) == 0
            % Dichotomia method
            fprintf('D\n');
            new_value = 10^(current_EbN0dB / 10) - (10^(current_EbN0dB / 10) - 10^(last_non_zero_EbN0dB / 10)) / 2;
          else
            % Secant method
            fprintf('S\n');
            new_value = 10^(visited_EbN0dB(end)/10) - (10^(visited_EbN0dB(end)/10) - 10^(visited_EbN0dB(end - 1)/10)) / (visited_EbN0dB_ber(end) - visited_EbN0dB_ber(end - 1)) * (visited_EbN0dB_ber(end) - T); 
          end
        end
        
        current_EbN0dB = 10*log10(new_value);
      else
        add_point(current_EbN0dB, sum(worker_total_points), sum(worker_nb_errors));
        fprintf('\n');
        break
      end
    end
  end
end