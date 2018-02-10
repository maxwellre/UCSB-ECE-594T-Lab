%% Display cursor position
close all
clear all
% -------------------------------------------------------------------------
% Import data
PosiFileName = 'CursorPosition.csv';
CursorPosi = csvread(PosiFileName);

fprintf('Sampling frequency = %.2f Hz\n',...
    size(CursorPosi,1)/CursorPosi(end,end));

% -------------------------------------------------------------------------
% Convert unit from m to mm
CursorPosi(:,1:end-1) = 1000*CursorPosi(:,1:end-1);

% -------------------------------------------------------------------------
% Display cursor axis trajectory
axisLabel = {'X','Y','Z'};
figure('Position',[80,80,800,600])
for ax = 1:3
    subplot(3,1,ax)
    plot(CursorPosi(:,end),CursorPosi(:,ax));
    ylabel(sprintf('%s-axis (mm)',axisLabel{ax}));
end
xlabel('Time (secs)');

% -------------------------------------------------------------------------
% Display cursor spatial trajectory
figure('Position',[780,80,800,600])
plot3(CursorPosi(:,1),CursorPosi(:,2),CursorPosi(:,3));
axis equal;
xlabel('X-axis (mm)');
ylabel('Y-axis (mm)');
zlabel('Z-axis (mm)');
