classdef ConstellationManager
  methods(Static)
    function C = give_constellation_data(constellation_data)
        C = struct();
        switch lower(constellation_data)
            case{'2-pam', 'bpsk', '2-psk'}
                Q = 1;
                
                % Same thing as 2 * b - 1
                A = {
                    {Q, pi, [0]}
                    {Q, 0,  [1]}
                };
            case{'uncentered-symbols'}
                Q = 0.5;
                
                % Same thing as 2 * b - 1
                A = {
                    {-Q, pi,   [0]}
                    {2 * Q, 0, [1]}
                };
            case{'4-pam'}
                Q = 1;
                
                A = {
                    {Q, pi,     [0 0]}
                    {Q / 2, pi, [0 1]}
                    {Q / 2, 0,  [1 1]}
                    {Q, 0,      [1 0]}
                };
            case{'qpsk', '4-psk'}
                Q = 1;
                
                A = {
                    {Q, pi/4,    [0 1]}
                    {Q, 3*pi/4,  [1 1]}
                    {Q, -3*pi/4, [1 0]}
                    {Q, -pi/4,   [0 0]}
                };
            case{'8-psk'}
                Q = 1;
                
                % Source of coding : https://fr.mathworks.com/help/comm/ug/gray-coded-8-psk.html
                A = {
                    {Q, 0,      [0 0 0]}
                    {Q, pi/4,   [0 0 1]}
                    {Q, 3*pi/4, [0 1 0]}
                    {Q, 2*pi/4, [0 1 1]}
                    {Q, 7*pi/4, [1 0 0]}
                    {Q, 6*pi/4, [1 0 1]}
                    {Q, 4*pi/4, [1 1 0]}
                    {Q, 5*pi/4, [1 1 1]}
                };
            case{'16-qam'}
                Q = 1;
                
                Q1 = 1 / 3 * Q;
                Q2 = 3 / 4 * Q;
                Q3 = Q;
                
                % Source of coding : http://erendemir.weebly.com/uploads/4/8/5/0/4850484/qpsk_and_16-qam_digital_modulation.pdf
                A = ConstellationManager.format_constellation({
                    {[1 1],     [1 1 1 1]}
                    {[1 -1],    [1 1 0 1]}
                    {[-1 -1],   [0 1 0 1]}
                    {[-1 1],    [0 1 1 1]}
                    ...
                    {[3 1],     [1 0 1 1]}
                    {[3 3],     [1 0 1 0]}
                    {[1 3],     [1 1 1 0]}
                    {[-1 3],    [0 1 1 0]}
                    ...
                    {[-3 3],    [0 0 1 0]}
                    {[-3 1],    [0 0 1 1]}
                    
                    {[-3 -3],   [0 0 0 0]}
                    {[-3 -1],   [0 0 0 1]}
                    ...
                    {[-1 -3],   [0 1 0 0]}
                    {[1 -3],    [1 1 0 0]}
                    {[3 -3],    [1 0 0 0]}
                    {[3 -1],    [1 0 0 1]}
                });
        end
        
        C.name = constellation_data;
        C.A = A;
        C.BPS = round(log2(length(A)));
        C.Q = Q;
    end
    
    function r = format_constellation(c)
        r = {};
        E = 0;
        M = length(c);
        
        for i=1:M
            p = c{i}{1}(1) + 1i * c{i}{1}(2);
            r{i} = {abs(p), angle(p), c{i}{2}};
            E = E + abs(p)^2;
        end
        for i=1:M
            r{i}{1} = r{i}{1} / sqrt(E / M);
        end
    end
    
    function [c, M] = apply_constellation(b, constellation_data, side)
        if nargin == 2
            side = 1;
        end
        
        if side == 2
            b = b.';
        end
        M = constellation_data.BPS;
        A = constellation_data.A;
        
        if mod(size(b, 2), M)
            error('Number of samples per symbol must be a factor of %d.', M);
        end
        
        c = zeros(size(b, 1), size(b, 2) / M);
        for i = 1:size(b, 1)
            v = reshape(b(i, :), M, size(b, 2) / M);
            cc = zeros(1, size(v, 2));
            for ii=1:size(v, 2)
                for iii = 1:length(A)
                    Ai = A{iii};
                    if Ai{3} == v(:, ii).'
                        cc(ii) = Ai{1} * exp(1i*Ai{2});
                        break
                    end
                end
                % Little fix used to fix MATLAB calculation errors
                % The fact is imag(exp(1i * pi)) != 0 (approx 1e-17)
                % So we just force it to zero
                if abs(real(c(ii))) < 1e-10, c(ii) = 1i * imag(c(ii)); end
                if abs(imag(c(ii))) < 1e-10, c(ii) = real(c(ii)); end
            end
            c(i, :) = cc;
        end
        
        if side == 2
            c = c.';
        end
    end
    function b = decode_constellation(y, constellation_data, side)
        if nargin == 2
            side = 1;
        end
        
        if side == 2
            y = y.';
        end
        
        b = zeros(size(y, 1), constellation_data.BPS * size(y, 2));
        for i=1:size(y, 1)
            bb = [];
            for j=1:size(y, 2)
                c = y(i, j);
                
                min_dist = inf;
                min_c = 0;
                
                for k = 1:length(constellation_data.A)
                    c2 = constellation_data.A{k};
                    d = abs(c2{1} * exp(1i * c2{2}) - c);
                    if d < min_dist
                        min_dist = d;
                        min_c = c2;
                    end
                end
                
                bb = [bb min_c{3}];
            end
            b(i, :) = bb;
        end
        
        if side == 2
            b = b.';
        end
    end
    
    function plot_constellation(ct)
      scatter(real(ct), imag(ct));
      grid on;
      xlim([-max(abs(ct)) max(abs(ct))] * 1.1)
      ylim([-max(abs(ct)) max(abs(ct))] * 1.1)
      
      title('Constellation');
    end
  end
end
