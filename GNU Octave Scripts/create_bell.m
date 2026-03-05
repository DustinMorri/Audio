function y = create_bell(width,outputRange)
	run = (width-1)/2;
	slope = 1.0/run;
	left = 0:slope:1.0;
	right = flip(left);
	if mod(width,2)==1
		right = right(2:end);
	else
	left_bell = soft_clip("sin",left,[0.0,1.0],outputRange);
	right_bell = soft_clip("sin",right,[0.0,1.0],outputRange);
	y = horzcat(left_bell,right_bell);
end