#version 400

struct LIGHT {
	vec4 position; // assume point or direction in EC in this example shader
	vec4 ambient_color, diffuse_color, specular_color;
	vec4 light_attenuation_factors; // compute this effect only if .w != 0.0f
	vec3 spot_direction;
	float spot_exponent;
	float spot_cutoff_angle;
	bool light_on;
};

struct MATERIAL {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	vec4 emissive_color;
	float specular_exponent;
};

uniform vec4 u_global_ambient_color;
#define NUMBER_OF_LIGHTS_SUPPORTED 4
uniform LIGHT u_light[NUMBER_OF_LIGHTS_SUPPORTED];
uniform MATERIAL u_material;

///////////////////////추가11/////////////////////
uniform float u_ratio;
uniform float u_screen;
uniform int u_flag_fog;
////////////////////////////////////////////////////
///////////추가2222(강의자료)/////////////////////////////
uniform bool screen_effect = false;
uniform float screen_width = 0.125f

uniform bool u_blind_effect = false;

uniform bool u_cartoon_effect = false;
uniform float u_cartoon_levels = 3.0f
////////////////////////////////////////////

const float zero_f = 0.0f;
const float one_f = 1.0f;

in vec3 v_position_EC;
in vec3 v_normal_EC;
in vec2 v_position_sc; //추가

layout (location = 0) out vec4 final_color;

vec4 lighting_equation(in vec3 P_EC, in vec3 N_EC) {
	vec4 color_sum;
	float local_scale_factor, tmp_float; 
	vec3 L_EC;

	color_sum = u_material.emissive_color + u_global_ambient_color * u_material.ambient_color;
 
	for (int i = 0; i < NUMBER_OF_LIGHTS_SUPPORTED; i++) {
		if (!u_light[i].light_on) continue;

		local_scale_factor = one_f;
		if (u_light[i].position.w != zero_f) { // point light source
			L_EC = u_light[i].position.xyz - P_EC.xyz;

			if (u_light[i].light_attenuation_factors.w  != zero_f) {
				vec4 tmp_vec4;

				tmp_vec4.x = one_f;
				tmp_vec4.z = dot(L_EC, L_EC);
				tmp_vec4.y = sqrt(tmp_vec4.z);
				tmp_vec4.w = zero_f;
				local_scale_factor = one_f/dot(tmp_vec4, u_light[i].light_attenuation_factors);
			}

			L_EC = normalize(L_EC);

			if (u_light[i].spot_cutoff_angle < 180.0f) { // [0.0f, 90.0f] or 180.0f
				float spot_cutoff_angle = clamp(u_light[i].spot_cutoff_angle, zero_f, 90.0f);
				vec3 spot_dir = normalize(u_light[i].spot_direction);

				tmp_float = dot(-L_EC, spot_dir);
				if (tmp_float >= cos(radians(u_light[i].spot_cutoff_angle))) {
					if (u_blind_effect)/////이 부분 추가임//////
					{
						tmp_float = pow(tmp_float,u_light[i].spot_exponent) * cos(90.0f * cos(tmp_float));
					}
					else
						tmp_float = pow(tmp_float, u_light[i].spot_exponent);

					if(tmp_float < zero_f)
						tmp_float = zero_f;
				}
				else 
					tmp_float = zero_f;
				local_scale_factor *= tmp_float;
			}
		}
		else {  // directional light source
			L_EC = normalize(u_light[i].position.xyz);
		}	

		if (local_scale_factor > zero_f) {				
			vec4 local_color_sum = u_light[i].ambient_color * u_material.ambient_color;

			tmp_float = max(zero_f, dot(N_EC, L_EC));  
			////////////tmp_float = dot(N_EC, L_EC);
			local_color_sum += u_light[i].diffuse_color*u_material.diffuse_color*tmp_float;
			
			vec3 H_EC = normalize(L_EC - normalize(P_EC));
			tmp_float = max(zero_f, dot(N_EC, H_EC)); 
			///////tmp_float = dot(N_EC, H_EC); 
			if (tmp_float > zero_f)
			{
				local_color_sum += u_light[i].specular_color*u_material.specular_color*pow(tmp_float, u_material.specular_exponent);
			}
			color_sum += local_scale_factor*local_color_sum;
		}
	}

 	return color_sum;
}

#define FOG_COLOR vec4(0.7f, 0.7f, 0.7f, 1.0f)
#define FOG_NEAR_DISTANCE 350.0f
#define FOG_FAR_DISTANCE 700.0f

void main(void) {   
	// final_color = vec4(gl_FragCoord.x/800.0f, gl_FragCoord.y/800.0f, 0.0f, 1.0f); // what is this?
    // final_color = vec4(0.0f,  0.0f, 1.0 - gl_FragCoord.z/1.0f, 1.0f); // what is this?

	////추가///////////////////////////////////
	float fog_factor;
	vec4 shaded_color;
	int temp = 4;
	int i,j;

	if(u_screen > zero_f)
	{
		for(i=0 ; i<u_screen ; i++)
		{
			if(s_position.x > (1.0 + i*2.0) / (u_screen*2 + 1.0) && s_position.x < (2.0+ i*2.0) / (u_screen*2 + 1.0))
			{
				for(j=0 ; j<u_screen ; j++)
				{
					if(s_position.y > (1.0+ j*2.0) / (u_screen*2.0 + 1.0) && s_position.y < (2.0 + j*2.0) / (u_screen*2.0 + 1.0))
						discard;
				}
			}
		}
	}

	if (u_flag_fog == 1) 
	{
		shaded_color = lighting_equation(v_position_EC, normalize(v_normal_EC));
		fog_factor = (FOG_FAR_DISTANCE - length(v_position_EC.xyz))/(FOG_FAR_DISTANCE - FOG_NEAR_DISTANCE);
		fog_factor = clamp(fog_factor, 0.0f, 1.0f);
		final_color = mix(FOG_COLOR, shaded_color, fog_factor);
	}
	else
		final_color = lighting_equation(v_position_EC, normalize(v_normal_EC)); // for normal rendering
}
