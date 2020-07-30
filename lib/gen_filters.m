function [y,t] = gen_filters(type,Ts,F,N,alpha)
    % GEN_FILTERS Return the impulse response of several pulse shaping filters
    % along with their time index vector.
    %
    % - type : 'nrz' for a gate of duration Ts
    %        'rz' for a gate of duration Ts/2
    %        'srrc' for a square root raised cosine of parameter alpha
    % - Ts : symbol duration [s]
    % - F  : samples per symbol
    % - N  : duration of the impulse response [Ts*s]
    % - alpha : roll-off factor ('srrc' only)
  
    t=-Ts*N/2:Ts/F:Ts*N/2;
  
    switch lower(type)
        case{'nrz'}
            y=[ones(1,F) zeros(1,length(t)-F)];
        case{'rz'}
            if mod(F,2)
                error('Number of samples per symbol must be a factor of two.');
            end
            
            y=1/sqrt(0.5)*[ones(1,F/2) zeros(1,length(t)-F/2)];
        case{'srrc'}
            % General case (abs(t) != T/(4*alpha))
            y=(1./(1-(4*alpha.*t/Ts).^2)).*((1-alpha)*sinc((t/Ts)*(1-alpha))+(4*alpha/pi).*cos((pi*t/Ts)*(1+alpha)));
            
            % Particular values (t=0 and abs(t) = T/(4*alpha))
            for indice=1:length(t)
                %if t(indice)==0
                if abs(t(indice)) < 10^(-17)
                    y(indice)=(1/sqrt(F))*(1-alpha+4*(alpha/pi));
                end
                if abs(abs(t(indice)) - Ts/(4*alpha)) < 10^(-17)
                    y(indice)=(alpha/sqrt(2*F))*((1+2/pi)*sin(pi/(4*alpha)) + (1-2/pi)*cos(pi/(4*alpha)));
                end
            end
    end
  end