/*Author: Dustin Morrison
Date: February 2026
Description:
	This script allows the user to change the audio side profile in ways that can make certain instruments and sounds really pop out of the mix.
	I got the idea for this by tinkering around with FL Studio's Patcher plug-in after reading about some plugins in an audio engineering magazine.
	I also wasn't quite satisfied with Izotope's Imager plug-in.
*/

//If you want to see what this function does, paste this LaTeX formula into the Desmos online graphing calculator.
//f\left(x\right)=\frac{x\left(a+1\right)}{a\left|x\right|+1}
//in - 0.0 to 1.0
//tension - 0.0 to 1.0 where 0.0 is a linear slope and 1.0 is a right angle
//returns values 0.0 to 1.0 closer to the bottom right of the graph depending on the tension amount
float curveCompress(float in,float tension){
	tension *= -1.0;
	return (in * (tension + 1.0)) / (tension * fabs(in) + 1.0);
}

/*This would be nice to use, but Formula's interpreter or compiler does weird things with that 1.0 constant.
//y=1-f\left(1-x\right)
float curveSaturate(float in,float tension){//0.08,0.0 returns 1.0, should return 0.08
	return 1.0 - curveCompress(1.0 - in,tension);//1.0 - curveCompress(0.92,0.0)
}*/

//This function normalizes to a value 0.0 to 1.0 any value in between startDistance and startDistance + length.
//This is used so that the curveCompress function's tension amount is relative to the size of the zone.
//in - 0.0 to 1.0
//startDistance - 0.0 to 1.0
//length - 0.0 to 1.0
//returns values 0.0 to 1.0
float normalize(float in,float startDistance,float length){
	return (in - startDistance) / length;
}

//This function just does the opposite of normalize.
//in - 0.0 to 1.0
//startDistance - 0.0 to 1.0
//length - 0.0 to 1.0
//returns values 0.0 to 1.0
float denormalize(float in,float startDistance,float length){
	return startDistance + in * length;
}

formula_main_stereo {
	//Split the mid and side signals.
    float mid = (input.left + input.right) / 2.0;
    float side = (input.left - input.right) / 2.0;
	
	//Get the user input.
	int relativePositioning = SWITCH_1;
	float startDistance;
	float endDistance;
	float inflectionPointDistance;
	float radius;
	if(relativePositioning == 0){
		startDistance = KNOB_1;
		endDistance = KNOB_2;
	}else{
		inflectionPointDistance = KNOB_1;
		radius = KNOB_2 / 2.0;
		startDistance = inflectionPointDistance - radius;
		endDistance = inflectionPointDistance + radius;
		if(startDistance < 0.0){
			radius = inflectionPointDistance;
			startDistance = 0.0;
			endDistance = inflectionPointDistance + radius;
		}else if(endDistance > 1.0){
			radius = 1.0 - inflectionPointDistance;
			startDistance = inflectionPointDistance - radius;
			endDistance = 1.0;
		}
	}
	float diameter = endDistance - startDistance;
	float inflectionPointCompression = KNOB_3;
	float inflectionPointX = startDistance + diameter * inflectionPointCompression;
	float inflectionPointY = startDistance + diameter * (1.0 - inflectionPointCompression);
	float zone1xLength = inflectionPointX - startDistance;
	float zone1yLength = inflectionPointY - startDistance;
	float zone2xLength = endDistance - inflectionPointX;
	float zone2yLength = endDistance - inflectionPointY;
	float zone1compression = KNOB_4;
	int zone1compress = 0;
	float zone1tension = (0.5 - zone1compression) * 2.0;
	if(zone1compression > 0.5){
		zone1compress = 1;
		zone1tension = (zone1compression - 0.5) * 2.0;
	}
	float zone2compression = KNOB_5;
	int zone2compress = 0;
	float zone2tension = (0.5 - zone2compression) * 2.0;
	if(zone2compression > 0.5){
		zone2compress = 1;
		zone2tension = (zone2compression - 0.5) * 2.0;
	}
	
	//Determine the output value.
	float sideSign = 0.0;
	if(side > 0.0){
		sideSign = 1.0;
	}else if(side < 0.0){
		sideSign = -1.0;
	}
	float absSide = fabs(side);
	if(absSide > startDistance && absSide < endDistance){
		float outSide = absSide;
		//If we're in zone 1,
		if(absSide < inflectionPointX){
			if(zone1compress == 1){
				outSide = denormalize(curveCompress(normalize(absSide,startDistance,zone1xLength),zone1tension),startDistance,zone1yLength);
			}else{
				float no = normalize(absSide,startDistance,zone1xLength);
				no = 1.0 - no;
				float cc = curveCompress(no,zone1tension);
				cc = 1.0 - cc;
				outSide = denormalize(cc,startDistance,zone1yLength);
			}
		}else if(absSide > inflectionPointX){
			if(zone2compress == 1){
				outSide = denormalize(curveCompress(normalize(absSide,inflectionPointX,zone2xLength),zone2tension),inflectionPointY,zone2yLength);
			}else{
				float no = normalize(absSide,inflectionPointX,zone2xLength);
				no = 1.0 - no;//formula tries to compile away too much...
				float cc = curveCompress(no,zone2tension);
				cc = 1.0 - cc;
				outSide = denormalize(cc,inflectionPointY,zone2yLength);
			}
		}
		side = sideSign * outSide;
	}
	
    //Protect the signal from the user's assertion of the usual maximum stereo amplitude.
	if(mid > 1.0){
		mid = 1.0;
	}else if(mid < -1.0){
		mid = -1.0;
	}
	if(side > 1.0){
		side = 1.0;
	}else if(side < -1.0){
		side = -1.0;
	}
	
	//Recombine the mid and side signals.
    Stereo output;
    output.left = mid + side;
    output.right = mid - side;
	return output;
}