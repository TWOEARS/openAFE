clear all; close all; clc;

addpath(genpath('/home/musabini/matlab_ws'));
addpath(genpath('/home/musabini/TwoEars/AuditoryModel/TwoEars-1.2'));
startTwoEars;

load ../../AFE_earSignals_16kHz.mat
load pp_bPreEmphasis.mat

requests = 'time';

dataObj = dataObject([],fsHz,10,2);

p = genParStruct('pp_bPreEmphasis',true);

% Create a manager
mObj_DC = manager(dataObj,requests,p);

% Request processing
mObj_DC.processChunk(earSignals);

time = timeVector( left', fsHz );

subplot ( 2,2,1 );
plot(time, mObj_DC.Data.time{1}.Data(:));
title('Left Channel (Matlab)'); 
ylabel('Amplitude');
xlabel('Time (s)');

subplot ( 2,2,2 );
plot(time, mObj_DC.Data.time{2}.Data(:));
title('Right Channel (Matlab)'); 
ylabel('Amplitude');
xlabel('Time (s)');

subplot ( 2,2,3 );
plot(time, left);
title('Left Channel (openAFE)'); 
ylabel('Amplitude');
xlabel('Time (s)');

subplot ( 2,2,4 );
plot(time, right);
title('Right Channel (openAFE)');
ylabel('Amplitude');
xlabel('Time (s)');
