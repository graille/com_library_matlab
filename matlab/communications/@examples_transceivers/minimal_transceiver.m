function [b_emitted, b_received] = minimal_transceiver(EbN0dB)
    if nargin == 0
      EbN0dB = 15;
    end
    
    % -----------------------------------------------------------------------------
    %% Configuration
    % -----------------------------------------------------------------------------

    % Simulation
    Rb = 32e3; % Bitrate (b/s)
    BPS = 1; % Bits per symbol (BPSK = 1, QPSK = 2, 8-PSK=3 ...)
    
    N_symb = 1000; % Number of symbols which will be sent
  
    % PSF configuration
    psf_type = 'NRZ'; % NRZ, RZ or SRRC
    psf_rolloff = 0.4; % Roll-off factor (For SRRC pulse shapping only)
    psf_N = 8; % duration of the filter impulse response [Ts*s]
    
    % Signal configuration
    Fsc = 64e3; % Sub-carrier frequency (Hz)
    
    % Computed constants
    Fs = ceil(8*(Rb + Fsc) / Rb) * Rb; % Sampling frequency -Manage value to have a multiple of Rb, for simplicity reasons linked to PSF- (Hz)
    nF = floor(Fs/Rb); % Number of samples per symbol
    
    % -----------------------------------------------------------------------------
    %% Emitter
    % -----------------------------------------------------------------------------
  
    % Generate symbols
    b = randi([0 2^BPS - 1], 1, N_symb);
    c = pskmod(b, 2^BPS, pi/BPS);
  
    % Upsample by a nF factor
    ct = upsample(c, nF);
  
    % Generate PSF
    [gt, ~] = gen_filters(psf_type, BPS / Rb, nF, psf_N, psf_rolloff);
  
    % Convolution used to generate signal
    cdt = conv(ct, gt);
    t = (0:length(cdt) - 1) / Fs;
  
    % -----------------------------------------------------------------------------
    %% Channel
    % -----------------------------------------------------------------------------

    % Basic modulation
    y = cdt .* exp(1i*2*pi*Fsc*t); 
    
    % Add AWGN noise
    yn = awgn(y, EbN0dB, 10*log10(Fs/Rb));

    % Recover signal
    yr = yn .* exp(1i * -2*pi*Fsc*t); % Basic demodulation

    % -----------------------------------------------------------------------------
    %% Receiver
    % -----------------------------------------------------------------------------
  
    % Generate matched filter and remove front tail induced by convolution
    rct = conv(yr, fliplr(gt));
    rct = rct(nF * psf_N + 1:end);
  
    % Decimation at eye diagram maximum likelihood
    selector = 1:nF:length(rct);
    selector = selector(1:length(c)); % Select the length(c) first symbols
    rc = rct(selector);
  
    % Demapping
    br = pskdemod(rc, 2^BPS, pi/BPS);
    
    % Convert int into bits
    b_emitted = reshape(de2bi(b, BPS, 'left-msb'), 1, BPS*N_symb);
    b_received = reshape(de2bi(br, BPS, 'left-msb'), 1, BPS*N_symb);
  end