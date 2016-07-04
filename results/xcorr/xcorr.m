clear all; close all; clc;

addpath(genpath('/home/musabini/matlab_ws'));
startTwoEars;

load ../AFE_earSignals_16kHz.mat

load xcorr_out.mat

%% PLACE REQUEST AND CONTROL PARAMETERS
% 
% 
% Request cross-corrleation function (CCF)
requests = {'crosscorrelation'};

dataObj = dataObject([],fsHz,10,2);

% Create a manager
mObj = manager(dataObj,requests);

% Request processing
mObj.processChunk(earSignals);

openAFE_left = CorrelationSignal(mObj.Processors{4},10,'mono',permute(leftOutput,[3 1 2]));

f1l = subplot(2,1,1);
dataObj.crosscorrelation{1}.plot(f1l);
f1r = subplot(2,1,2);
openAFE_left.plot(f1r);


