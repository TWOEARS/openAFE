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

openAFE_left = CorrelationSignal(mObj.Processors{4},10,'mono',leftOutput);

% f1l = subplot(2,1,1);
% dataObj.crosscorrelation{1}.plot(f1l);
% f1r = subplot(2,1,2);
% openAFE_left.plot(f1r);

%% Plottings

wavPlotZoom = 5; % Zoom factor
wavPlotDS   = 1; % Down-sampling factor

% Summarize plot parameters
p = genParStruct('wavPlotZoom',wavPlotZoom,'wavPlotDS',wavPlotDS);

% Plot the CCF of a single frame
frameIdx2Plot = 10;

% Get sample indexes in that frame to limit waveforms plot
wSizeSamples = 0.5 * round((cc_wSizeSec * fsHz * 2));
wStepSamples = round((cc_hSizeSec * fsHz));
samplesIdx = (1:wSizeSamples) + ((frameIdx2Plot-1) * wStepSamples);

lagsMS = dObj.crosscorrelation{1}.lags*1E3;

% Plot the waveforms in that frame
dObj.plot([],[],'bGray',1,'rangeSec',[samplesIdx(1) samplesIdx(end)]/fsHz)
ylim([-0.35 0.35])

f1m = subplot(4,1,1);
% Plot the cross-correlation in that frame
dObj.crosscorrelation{1}.plot([],p,frameIdx2Plot);
f1o = subplot(4,1,2);
openAFE_left.plot([],p,frameIdx2Plot);
title('Crosscorrelation computation (openAFE)');