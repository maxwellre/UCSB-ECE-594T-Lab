%% Display cursor position
close all
clear all
% -------------------------------------------------------------------------
% Import data
PosiFileName = 'EventData.csv';
eventData = csvread(PosiFileName);

Fs = 1000;
Ts = 1/Fs;

% -------------------------------------------------------------------------
% Display cursor axis trajectory
axisLabel = {'X','Y','Z'};
figure('Position',[80,80,800,600])
plot(eventData(:,1)*Ts,eventData(:,2));
ylabel('Amplitude');
% xlabel('Time (secs)');
