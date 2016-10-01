%% Read IMU's data, sent by Arduino, through serial
close all;
clc;

% set(0, 'DefaultFigureWindowStyle', 'docked');
%%
clc;
flag = [0 1 0];                     % [accelerometer gyroscope temperature]
fd = serial('/dev/ttyUSB0');

err_x_acc = 0;
err_x_acc_old = 0;
x_acc_filtered = 0;
err_y_acc = 0;
err_y_acc_old = 0;
y_acc_filtered = 0;
err_z_acc = 0;
err_z_acc_old = 0;
z_acc_filtered = 0;

close all;
max_size = 500;
t = 1:max_size;

raw_x_acc = zeros(max_size, 1);
raw_y_acc = zeros(max_size, 1);
raw_z_acc = zeros(max_size, 1);
raw_temp = zeros(max_size,1);
raw_x_gyro = zeros(max_size,1);
raw_y_gyro = zeros(max_size,1);
raw_z_gyro = zeros(max_size,1);

filt_x_acc = zeros(max_size,1);
filt_y_acc = zeros(max_size,1);
filt_z_acc = zeros(max_size,1);
filt_temp = zeros(max_size,1);
filt_x_gyro = zeros(max_size,1);
filt_y_gyro = zeros(max_size,1);
filt_z_gyro = zeros(max_size,1);

sum_x_acc = 0;
sum_y_acc = 0;
sum_z_acc = 0;
sum_temp = 0;
sum_x_gyro = 0;
sum_y_gyro = 0;
sum_z_gyro = 0;

% Prepare the plot
timeInterval = 0.001;
%
if (flag(1))
    figure(1);
    rax = plot(t, raw_x_acc, 'r');
    hold on;
    ray = plot(t, raw_y_acc, 'g');
    raz = plot(t, raw_z_acc, 'b');
    grid on;
end
%
if (flag(2))
    figure(2);
    rgx = plot(t, raw_x_gyro, 'r');
    hold on;
    rgy = plot(t, raw_y_gyro, 'g');
    rgz = plot(t, raw_z_gyro, 'b');
    grid on;
end
%
if (flag(3))
    figure(3);
    tm = plot(t, raw_temp, 'g');
    grid on;
end
%%
fopen(fd);
for i = 3:max_size
    A = fscanf(fd);
    B = textscan(A, '%d %d %d %f %d %d %d');
    [raw_x_acc(i), raw_y_acc(i), raw_z_acc(i), raw_temp(i), raw_x_gyro(i), raw_y_gyro(i), raw_z_gyro(i)] = deal(B{:});
    
    filt_x_acc(i) = 1.8*filt_x_acc(i-1) - 0.81*filt_x_acc(i-2) + 0.0022676*(raw_x_acc(i) + 2*raw_x_acc(i-1) + raw_x_acc(i-2));
    filt_y_acc(i) = 1.8*filt_y_acc(i-1) - 0.81*filt_y_acc(i-2) + 0.0022676*(raw_y_acc(i) + 2*raw_y_acc(i-1) + raw_y_acc(i-2));
    filt_z_acc(i) = 1.8*filt_z_acc(i-1) - 0.81*filt_z_acc(i-2) + 0.0022676*(raw_z_acc(i) + 2*raw_z_acc(i-1) + raw_z_acc(i-2));
    
    filt_x_gyro(i) = 1.8*filt_x_gyro(i-1) - 0.81*filt_x_gyro(i-2) + 0.0022676*(raw_x_gyro(i) + 2*raw_x_gyro(i-1) + raw_x_gyro(i-2));
    filt_y_gyro(i) = 1.8*filt_y_gyro(i-1) - 0.81*filt_y_gyro(i-2) + 0.0022676*(raw_y_gyro(i) + 2*raw_y_gyro(i-1) + raw_y_gyro(i-2));
    filt_z_gyro(i) = 1.8*filt_z_gyro(i-1) - 0.81*filt_z_gyro(i-2) + 0.0022676*(raw_z_gyro(i) + 2*raw_z_gyro(i-1) + raw_z_gyro(i-2));
    
    % Acceleration
    if (flag(1))
        set(rax, 'XData', t, 'YData', raw_x_acc);
        set(ray, 'XData', t, 'YData', raw_y_acc);
        set(raz, 'XData', t, 'YData', raw_z_acc);
    end
    % Gyroscope
    if (flag(2))
        set(rgx, 'XData', t, 'YData', raw_x_gyro);
        set(rgy, 'XData', t, 'YData', raw_y_gyro);
        set(rgz, 'XData', t, 'YData', raw_z_gyroc);
    end
    % Temperature
    if (flag(3))
        set(tm, 'XData', t, 'YData', raw_temp);
    end
    
    drawnow;
end
if (flag(1))
    figure(1);
    legend('Acc_x', 'Acc_y', 'Acc_z');
    hold off;
end
if (flag(2))
    figure(2);
    legend('Gyro_x', 'Gyro_y', 'Gyro_z');
    hold off;
end
if (flag(3))
    figure(3);
    legend('Temp');
    hold off;
end
if (flag(1))
    figure(11)
    plot(t, raw_x_acc/16384); grid on;
    hold on
    plot(t, filt_x_acc/16384);
    legend('Raw', 'Filtered');
    figure(12)
    plot(t, raw_y_acc/16384); grid on;
    hold on
    plot(t, filt_y_acc/16384);
    legend('Raw', 'Filtered');
    figure(13)
    plot(t, raw_z_acc/16384); grid on;
    hold on
    plot(t, filt_z_acc/16384);
    legend('Raw', 'Filtered');
end
if (flag(2))
    figure(21)
    plot(t, raw_x_gyro/131); grid on;
    hold on
    plot(t, filt_x_gyro/131);
    legend('Raw', 'Filtered');
    figure(22)
    plot(t, raw_y_gyro/131); grid on;
    hold on
    plot(t, filt_y_gyro/131);
    legend('Raw', 'Filtered');
    figure(23)
    plot(t, raw_z_gyro/131); grid on;
    hold on
    plot(t, filt_z_gyro/131);
    legend('Raw', 'Filtered');
end
fclose(fd);