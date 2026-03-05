%{
soft_clip.m

This function applies a soft clipping to an input vector based on specified input and output ranges. The clipping function can be customized.

Parameters:
   shape       - Standard sigmoid functions tanh, tanhmax (goes to 1.0 instead of 0.7616), alg, sin, or lin for linear (just renormalization).
   x           - The input vector to be clipped.
   inputRange  - A two-element vector specifying the range of the input values.
   outputRange - A two-element vector specifying desired output range.

Example usage:
   x = [0.0,0.2,0.4,0.6,0.8,1.0];
   y = soft_clip("tanh",x,[0.0,1.0],[0.0,1.0]);
   disp(y);
   y = soft_clip("lin",x(x >= 0.6),[0.6,1.0],[0.6,2.0]);
   disp(y);
%}

function y = soft_clip(shape, x, inputRange, outputRange)
	normalizedX = (x - inputRange(1)) / (inputRange(2) - inputRange(1));
	switch shape
		case "tanh"
			clippedNX = tanh(normalizedX);
		case "tanhmax"
			clippedNX = tanh(normalizedX) * 1.3130352855;
		case "alg"
			clippedNX = 2 * (normalizedX ./ (1 + abs(normalizedX)));
		case "sin"
			clippedNX = sin((pi/2)*normalizedX);
		case "lin"
			clippedNX = normalizedX;
	endswitch
	y = (outputRange(2) - outputRange(1)) * clippedNX + outputRange(1);
end