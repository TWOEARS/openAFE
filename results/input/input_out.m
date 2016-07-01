clear all; close all; clc;

load AFE_earSignals_16kHz.mat
load input_out.mat

input_left = TimeDomainSignal.construct(fsHz,10,'input','Ear Signals','left',earSignals(:,1)); 
input_right = TimeDomainSignal.construct(fsHz,10,'input','Ear Signals','right',earSignals(:,2)); 

openAFE_left = TimeDomainSignal.construct(fsHz,10,'input','Ear Signals (openAFE)','left',outputSignals(:,1)); 
openAFE_right = TimeDomainSignal.construct(fsHz,10,'input','Ear Signals (openAFE)','right',outputSignals(:,2)); 

f1l = subplot(2,2,1);
input_left.plot(f1l);
f1r = subplot(2,2,2);
input_right.plot(f1r);
f2l = subplot(2,2,3);
openAFE_left.plot(f2l);
f2r = subplot(2,2,4);
openAFE_right.plot(f2r);
