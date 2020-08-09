To test a transceiver, you can use ber_evaluator library

Example using 4 parallel workers:

```matlab
clear all; close all;
EbN0dBs = -8:1:8;
[ber, EbN0dBs] = ber_evaluator(@minimal_transceiver, EbN0dBs, 100, 4);

figure;
semilogy(EbN0dBs, ber, 'o-', 'DisplayName', 'Simulation'); hold on;

% Plot theorical BPSK curve to compare
bep = qfunc(sqrt(2*10.^(EbN0dBs/10)));
semilogy(EbN0dBs, bep, 'DisplayName', 'Theorical BPSK');
legend
grid on;
```

Example for the simple_transceiver:

```matlab
figure;
tr = @(e) simple_transceiver(e, false, 0);
[ber, EbN0dBs] = ber_evaluator(tr, 0:8, 1000, 12);
x = 0:0.1:9;
bep = qfunc(sqrt(2*10.^(x/10)));
semilogy(x, bep, 'DisplayName', 'BPSK theory'); hold on;
semilogy(EbN0dBs, ber, '-o', 'DisplayName', 'BPSK practice (no PM)');
legend;
grid;
xlabel('Eb/N0 (dB)');
ylabel('BER');

figure;
x = 0:0.1:9;
bep = qfunc(sqrt(2*10.^(x/10)));
semilogy(x, bep, 'DisplayName', 'BPSK theory'); hold on;

for h = [0.001 0.01 0.1 0.3 1 1.2 2]
  tr = @(e) simple_transceiver(e, true, h);
  [ber, EbN0dBs] = ber_evaluator(tr, 0:8, 2000, 12);
  semilogy(EbN0dBs, ber, '-o', 'DisplayName', sprintf('BPSK practice (h = %4.3f)', h));
end

legend;
grid;
xlabel('Eb/N0 (dB)');
ylabel('BER');
```

Note: If you don't have the parallel-toolbox, just change the parfor by a for loop inside ber_evaluator file
