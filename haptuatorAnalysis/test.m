n_steps = 1000;
interval = 5;
f_start = 10;
f_end = 1000;
y = NaN(n_steps,1);
for i = 1:n_steps
    delta = i / n_steps;
    t = interval * delta;
    phase = 2 * pi * t * (f_start + (f_end - f_start) * delta / 2);
    y(i) = 3* sin(phase);  
end
