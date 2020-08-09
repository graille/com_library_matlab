function [b_emitted, b_received] = simple_pm_transceiver(EbN0dB, activate_PM, h)
  %{
    EbN0dB: Value of Eb/N0 (in dB)
    activate_PM: boolean used to activate the PM modulation
    h: Modulation index (rad). Only used if activate_PM is true
  %}

  %% Parameters
  Fsc = 64e3; % Sub-carrier frequency (Hz)
  Rb = 16e3; % Bitrate (b/s)
  Fs = 4 * (Fsc + Rb); % Sampling frequency
  N = 3000; % Number of bits sent
  
  %% Emitter
  nF = Fs / Rb; % Number of samples per symbol
  b_emitted = round(rand(1, N)); % Create bits
  c = pskmod(b_emitted, 2); % BPSK modulation
  
  d = reshape(repmat(c, nF, 1), 1, nF * length(c)); % NRZ shapping
  t = (0:length(d) - 1) * 1/Fs;

  ye = d .* exp(2 * 1i * pi * Fsc .* t); % Put BPSK on sub-carrier
  
  % Phase modulation
  if activate_PM
    ye = exp(1i * h .* real(ye));
  end
  
  %% Add AWGN
  if activate_PM
    Pn = Fs/Rb * var(c) / 10^(EbN0dB / 10) * 2 * besselj(1, h)^2;
  else
    Pn = Fs/Rb * var(c) / 10^(EbN0dB / 10);
  end
  
  y = ye + sqrt(Pn / 2) .* (randn(1, length(ye)) + 1i .* randn(1, length(ye)));
  
  %% Receiver
  if activate_PM
    y = angle(y); % Recover angle
  end
  
  y = y .* exp(-2 * 1i * pi * Fsc .* t);
  cr = zeros(1, N);
  
  % Integrate and dump filter
  for ii = 0:N-1
    cr(ii + 1) = mean(y(ii * nF + 1:(ii + 1)*nF));
  end
  
  % Demodulation
  b_received = pskdemod(cr, 2);
end
