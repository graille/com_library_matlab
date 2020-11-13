function [fnt_cdf, fnt_esp_cdf, fnt_pdf] = distribution_properties(distribution_name)
  switch lower(distribution_name)
    case 'arcsin'
      fnt_esp_cdf = @(x, a, b) - 1/(2 * pi) * ((2 * sqrt(b - x) * sqrt(x - a)) - (a + b)*asin((2.*x - (a + b)) / (b - a)));
      fnt_cdf = @(x, a, b) 2 / pi * asin(sqrt((x - a) / (b - a)));
      fnt_pdf = @(x, a, b) 1 ./ (pi * sqrt((x - a) .* (b - x))); 

    case 'normal'
      fnt_esp_cdf = @(x, mu, s) 0.5 * mu * erf((mu - x) ./ (s*sqrt(2))) - (s*exp(-(x - mu).^2 ./ (2 * s^2)) / (sqrt(2 * pi)));
      fnt_cdf = @(x, mu, s) 0.5 .* (1 + erf((x - mu) ./ (s*sqrt(2))));
      fnt_pdf = @(x, mu, s) 1 ./ (s .* sqrt(2*pi)) .* exp(-(x - mu).^2 ./ (2*s^2));
    
    case 'exponential'
      fnt_esp_cdf = @(x, l) -1 * exp(-l .* x) .* (l .* x + 1) ./ l;
      fnt_cdf = @(x, l) 1 - exp(-l .* x);
      fnt_pdf = @(x, l) l .* exp(-l .* x);

    case 'laplace'
      fnt_esp_cdf = @(x, mu, b) 0.25 .* (b - x) .* exp((x - mu) / b) .* (sign(x - mu) - 1) - 0.25 .* (b + x) .* exp((mu - x) / b) .* (sign(x - mu) + 1);
      fnt_cdf = @(x, mu, b) - 0.25 * exp((x - mu) / b) .* (sign(x - mu) - 1) + 0.5 * (1 - 0.5*exp(-(x - mu) / b)) * (sign(x - mu) + 1);
      fnt_pdf = @(x, mu, b) 1 / (2 * b) .* exp(- abs(x - mu) / b);
  end
end