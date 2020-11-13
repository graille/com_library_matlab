function [xi, ti] = lloyd_max(Nq, t_min, t_max, fnt_cdf, fnt_esp_cdf, iterations)
  % Execute Lloyd-max algorithm on distribution described by fnt_cdf, fnt_esp_cdf where
  %   Input arguments:
  %     - Nq: Number of quantization bits.
  %     - t_min: Lower bound of initial thresholds array
  %     - t_max: Upper bound of initial thresholds array
  %     - fnt_cdf: Primitive of f(x), where f(x) is the density function
  %     - fnt_esp_pdf: Primitive of x.f(x) where f(x) is the density function
  %     - iterations: Number of iterations for the algorithm (default: 10000)

  if nargin < 6, iterations = 1e5; end
  
  % Start with uniform quantization
  xi = zeros(iterations, 2^Nq);
  
  for q = 0:(2^Nq - 1)
    xi(1, q + 1) = q * (t_max - t_min) / (2^Nq - 1) + t_min;
  end
  
  for i = 1:(iterations - 1)
    ti = [t_min (1 / 2 * (xi(i, 1:end - 1) + xi(i, 2:end))) t_max];
    new_line = zeros(1, 2^Nq);
    for q = 1:(2^Nq)
      new_line(q) = (fnt_esp_cdf(ti(q + 1)) - fnt_esp_cdf(ti(q))) / (fnt_cdf(ti(q + 1)) - fnt_cdf(ti(q)));
    end
    
    xi(i + 1, :) = new_line;
  end
end