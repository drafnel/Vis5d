#ifdef	HAVE_MIXKIT

#ifndef	_ncar_decimate_h_
#define	_ncar_decimate_h_

#ifdef __cplusplus
extern "C" {
#endif



void	DecimateTriStrip(
	const float	*vr,
	const float	*vc,
	const float	*vl,
	const float	*nx,
	const float	*ny,
	const float	*nz,
	int		numverts,
	const int	*vpts,
	int		numindexes,
	float		*deci_vr,
	float		*deci_vc,
	float		*deci_vl,
	float		*deci_nx,
	float		*deci_ny,
	float		*deci_nz,
	int		deci_max_tris,
	int		*deci_numverts
);

#ifdef __cplusplus
}
#endif


#endif	//	_ncar_decimate_h_

#endif	//	HAVE_MIXKIT
