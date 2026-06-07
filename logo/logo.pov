#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "skies.inc"

object {
	O_Cloud1
}

sky_sphere {
	pigment {
		gradient y
		color_map {
			[0.0 White*0.8]
			[0.2 rgb<0.2 0.2 0.7>]
		}
	}
}

#declare R = 60 + 45;

object {
	Cube

	pigment {
		color <1, 1, 0>
	}

	scale <0.75, 0.75, 0.75>
	rotate <R, R, R>
}

plane {
	y, -1
	pigment {
		checker Red Gray
	}
	finish {
		phong 1
		reflection 0.5
	}
}

light_source {
	<-250, 250, -250>
	color White*2
	parallel point_at <0, 0, 0>
}

camera {
	location <4, 1, -4>
	look_at <0, 0.25/8, 0>
	angle 35
	up <0, 1, 0>
	right image_width / image_height * x
}
