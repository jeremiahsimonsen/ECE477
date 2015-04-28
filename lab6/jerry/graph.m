adc = importdata('temp.txt');
adc(1:611) = [];
adc(5761:end) = [];
adc = (adc - 319)/1.2;
plot(adc);

for i=1:32:length(adc)
	xbar(ceil(i/32)) = mean(adc(i:i+31));
	n(ceil(i/32)) = i + 16;
end
hold on;
scatter(n,xbar,'r');
xlabel('Sample index');
ylabel('Temperature, ^\circC');
title('Temperature over time');
legend('Measured temperature','Averaged','location','southeast');