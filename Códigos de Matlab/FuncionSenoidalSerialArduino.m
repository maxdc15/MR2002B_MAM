clc
clearvars
serialPort = 'COM4'; 
baudRate = 9600;
s = serialport(serialPort, baudRate);

fs = 100;       
f = 1;           
t = 0:1/fs:2*pi;

sinusoidal_signal = sin(2*pi*f*t);
figure;
hold on;
grid on;
xlabel('Tiempo (s)');
ylabel('Señal');


receivedData = nan(size(sinusoidal_signal));

for i = 1:length(sinusoidal_signal)
    
    writeline(s, num2str(sinusoidal_signal(i)));
  
    plot(t(i), sinusoidal_signal(i), 'b.-');
    
  
    if s.NumBytesAvailable > 0
        data = str2double(readline(s));
        receivedData(i) = data;
       
        plot(t(i), data, 'r.-');
    end

    pause(0.1);
    

    drawnow;
end

disp('Transmisión completa.');

clear s;