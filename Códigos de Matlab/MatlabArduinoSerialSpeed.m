clc
clearvars

% Configuración de la comunicación serial con la placa Arduino
serialPort = 'COM4'; 
baudRate = 2000000;
s = serialport(serialPort, baudRate);

% Parámetros de la señal
f = 0.06;             % Frecuencia de la señal

% Definir duración de transmisión en segundos
duration = 15;

% Inicialización de los vectores para almacenar los datos enviados y recibidos
sentData = [];
receivedData = [];
timeData = [];  % Vector para almacenar el tiempo real

tic;  % Iniciar temporizador
disp('Transmisión comenzada.');
while toc < duration
    % Calcular el tiempo actual
    t_current = toc;
    
    % Generar la señal sinusoidal en el tiempo actual
    sinusoidal_signal = 1*sin(2 * pi * f * t_current);

    %sinusoidal_signal = 0.5*(t_current > 1);
    
    % Enviar dato por el puerto serial
    writeline(s, num2str(sinusoidal_signal));
    
    % Almacenar el dato enviado
    sentData = [sentData, sinusoidal_signal];
    
    %timeData = [timeData, t_current];  % Almacenar el tiempo actual
    
    % Comprobar si hay datos recibidos
    if s.NumBytesAvailable > 0
        data = str2double(readline(s));
        receivedData = [receivedData, data];  % Almacenar el dato recibido
    else
        receivedData = [receivedData, NaN];  % Si no hay datos, guardar NaN
    end
    timeData = [timeData, t_current];
    % Pausa para sincronizar con la recepción (opcional, ajustar según sea necesario)
    pause(0.001);
end

% Enviar señal de finalización
writeline(s, num2str(0));

% Indicar que la transmisión está completa
disp('Transmisión completa.');

% Limpiar el puerto serial
clear s;

% Graficar las señales enviada y recibida con respecto al tiempo real
figure;
hold on;
grid on;
plot(timeData, sentData, 'b', 'DisplayName', 'u(t)');   % azul para los datos enviados
plot(timeData, receivedData, 'r', 'DisplayName', 'ω(t)');  % rojo para los datos recibidos
legend;
xlabel('Tiempo (s)');
ylabel('Señal');
title('Comparación de señales enviada y recibida');

Sampling_Time_done = mean(diff(timeData));
disp("Tiempo de muestreo = " + Sampling_Time_done  + " seg")
disp("Frecuencia de muestreo = " + (1/Sampling_Time_done) + " Hz")