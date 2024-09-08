clc; close all; clearvars;

% Declarar t_current como global
global t_current;

% Configuración del puerto serial
serialPort = 'COM9';
baudRate = 2000000;
s = serialport(serialPort, baudRate);
amp = 2;

% Parámetros del PID
Kp = 0.09;   

Ki = 0.8;   

Kd = 0.002;   
Ts = 0.01;   

% Parámetros de la señal senoidal
f = 0.2; % Frecuencia de la señal senoidal en Hz
duration = 15; % Duración total de la simulación en segundos

% Crear objeto timer
tobj = timer;
tobj.UserData = struct('tStart', tic, 'sentData', [], 'receivedData', [], 'pidOutput', [], 'Ie', 0, 'e_1', 0, 'timeData', []);
tobj.TimerFcn = {@pidCallback, s, Kp, Ki, Kd, Ts, f, amp};  
tobj.StopFcn = {@stopMotor, s};  
tobj.StartFcn = @(~,~)disp('Control PID iniciado.');  
tobj.Period = Ts;
tobj.ExecutionMode = 'fixedRate';

% Iniciar el timer
start(tobj);
pause(duration);  
stop(tobj);  

% Procesamiento de los datos después de detener el timer
Y = tobj.UserData.receivedData;
T = tobj.UserData.timeData;

% Calcular el valor final y otros parámetros
Y_30 = zeros(1, 30);

Y_30 = Y(end-100:end);

% Calcular el valor final
Yfin = mean(Y_30);

disp("El valor final es = " + num2str (Yfin))

% Error en estado estacionario
Eee = abs(amp - Yfin);
disp("El Error en estado estacionario es = " + num2str (Eee))

% Tiempo de establecimiento
Elim = 0.02 * Yfin;
ts = T(end);

for i = 1:1:length(T)
    
    if abs(Y(i)-Yfin) > Elim
        ts = T(i);
    else
        break;
    end
end

disp("El ts = " + num2str (ts) + " segundos")

% Valor pico y Tiempo pico

[Ymax, indice] = max(Y);
tp = T(indice);

if Ymax > Yfin
    Mp = ((Ymax - Yfin) / Yfin) * 100;
else
    Mp = 0;
end

if tp> ts
    tp = 0;

end
disp("El tp es = " + num2str (tp) + " segundos")

disp("El Mp es = " + num2str (Mp) + "%")

% Tiempo de crecimiento
tr = T(end);

for i = 1:length(T)
    if Y(i) >= Yfin
        tr = T(i);
        break;
    end
end

disp("El tr es = " + num2str (tr) + " segundos")

R = amp;


% Integral del Error Cuadrático (ISE)
E = R - Y;
%plot(T,E,'b.-')
ISE = 0;

for i = 2:length(Y)
    ISE = ISE + (E(i)^2 + E(i-1)^2) * (T(i)-T(i-1))/2;
end

disp("ISE = " + num2str (ISE))

% Integral del Error Absoluto (IAE)
IAE = 0;

for i = 2:length(Y)
    IAE = IAE + (abs(E(i)) + abs(E(i-1))) * (T(i)-T(i-1))/2;
end

disp("IAE = " + num2str (IAE))

% Integral del Tiempo y Error Cuadrático (ITSE)
ITSE = 0;

for i = 2:length(Y)
    ITSE = ITSE + (T(i)*E(i)^2 + T(i-1)*E(i-1)^2) * (T(i)-T(i-1))/2;
end

disp("ITSE = " + num2str (ITSE))

% Integral del Tiempo y Error Absoluto (ITAE)
ITAE = 0;

for i = 2:length(Y)
    ITAE = ITAE + (T(i)*abs(E(i)) + T(i-1)*abs(E(i-1))) * (T(i)-T(i-1))/2;
end

disp("ITAE = " + num2str (ITAE))

delete(tobj);

% Cerrar el puerto serial
flush(s);
clear s;

% Callback del timer
function pidCallback(tobj, ~, s, Kp, Ki, Kd, T, f, amp)
    % Declarar t_current como global
    global t_current;
    
    % Calcular el tiempo actual
    t_current = toc(tobj.UserData.tStart);
    
    % Generar la señal senoidal deseada
    desired = 0 * (t_current > 3) + 2 * sin(2 * pi * f * t_current);
    

    % Leer el valor del sensor
    y = readSensor(s);
    
    % Calcular el error
    e = desired - y;
    
    % Integral del error
    Ie = tobj.UserData.Ie + (e + tobj.UserData.e_1) * T / 2;
    
    % Derivada del error
    De = (e - tobj.UserData.e_1) / T;
    
    % Calcular la salida del PID
    u = e * Kp + Ie * Ki + De * Kd;
    
    % Saturación de la señal de control
    u = saturacion(u);
    
    % Enviar la salida al actuador
    writeActuator(s, u);
    
    % Almacenar los datos
    tobj.UserData.sentData = [tobj.UserData.sentData, desired];
    tobj.UserData.receivedData = [tobj.UserData.receivedData, y];
    tobj.UserData.pidOutput = [tobj.UserData.pidOutput, u];
    tobj.UserData.Ie = Ie; 
    tobj.UserData.e_1 = e;  
    tobj.UserData.timeData = [tobj.UserData.timeData, t_current];
end

function stopMotor(~, ~, s)
    writeline(s, num2str(0)); 
    disp('Control PID finalizado.');
    
    % Recuperar datos del objeto timer
    sentData = evalin('base', 'tobj.UserData.sentData');
    receivedData = evalin('base', 'tobj.UserData.receivedData');
    pidOutput = evalin('base', 'tobj.UserData.pidOutput');
    timeData = evalin('base', 'tobj.UserData.timeData');
    
    figure;
    hold on;
    grid on;
    plot(timeData, sentData, 'b', 'DisplayName', 'Setpoint');  
    plot(timeData, receivedData, 'r', 'DisplayName', 'Measured Velocity'); 
    plot(timeData, pidOutput, 'g', 'DisplayName', 'PID Output'); 
    legend;
    xlabel('Tiempo (s)');
    ylabel('Señal');
    title('Comparación de señales del control PID');
    
    Sampling_Time_done = mean(diff(timeData));
    disp("Tiempo de muestreo = " + Sampling_Time_done + " seg");
    disp("Frecuencia de muestreo = " + (1/Sampling_Time_done) + " Hz");
end

function y = readSensor(s)
    if s.NumBytesAvailable > 0
        y = str2double(readline(s));
    else
        y = 0;  
    end
end

function writeActuator(s, u)
    writeline(s, num2str(u)); 
end

function u_sat = saturacion(u)
    u_max = 1;  
    u_min = -1;    
    u_sat = min(max(u, u_min), u_max);  
end
