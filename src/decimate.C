#include "../config.h"
#ifdef HAVE_MIXKIT
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include <mixkit/stdmix.h>
#include <mixkit/mixio.h>
#include <mixkit/MxTimer.h>
#include <mixkit/MxSMF.h>
#include <mixkit/MxQSlim.h>
#include "decimate.h"



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
) {
	MxStdModel      *m;
	double          init_time, slim_time;
	int             i, j, vi;

	m = new MxStdModel(16, 32);
	m->normal_binding(MX_PERVERTEX);

	for (i=0; i<numverts; i++) {
		m->add_vertex(vr[i], vc[i], vl[i]);
		m->add_normal(nx[i], ny[i], nz[i]);
	}

	for (i=0; i<numindexes-2; i++) {
		m->add_face(vpts[i], vpts[i+1], vpts[i+2]);
	}

	MxEdgeQSlim slim(*m);

	MXTIME(init_time, slim.initialize());
	MXTIME(slim_time, slim.decimate(deci_max_tris));

	// First, mark stray vertices for removal
	//
	for(i=0; i<m->vert_count(); i++) {
		if(m->vertex_is_valid(i) &&
			m->neighbors(i).length() == 0) {

		m->vertex_mark_invalid(i);
		}
	}

	// Compact vertex array so only valid vertices remain
	m->compact_vertices();

#ifdef DEBUG
	cerr << "+ Running time" << endl;
	cerr << "    PSlim init  : " << init_time << " sec" << endl;
	cerr << "    PSlim run   : " << slim_time << " sec" << endl;
	cerr << endl;
	cerr << "    PSlim Total : " << init_time+slim_time << endl;
	cerr << "    Total       : " << init_time+slim_time<<endl;
	cerr << endl;

	cerr << "    Original geometry : " << numindexes-2  << " tris, "
		<< numverts * 3 << " verts" << endl;

	cerr << "    Slimmed geometry : " << slim.valid_faces << " tris, "
		<< slim.valid_verts << " verts" << endl;
	cerr << "    Slimmed geometry : " << slim.valid_faces << " tris, "
		<< m->vert_count() << " verts" << endl;
#endif

	for(i=0, vi=0; i<m->face_count(); i++) {

                if( m->face_is_valid(i) ) {
			MxFace		f;
			MxVertex	v;
			MxNormal	n;

                        f = m->face(i);

			for(j=0; j<3; j++) {
				v = m->vertex(f[j]);
				n = m->normal(f[j]);

				deci_vr[vi] = v[0];
				deci_vc[vi] = v[1];
				deci_vl[vi] = v[2];

				deci_nx[vi] = n[0];
				deci_ny[vi] = n[1];
				deci_nz[vi] = n[2];
				vi++;
			}
		}
	}
	*deci_numverts = vi;

	delete m;
}
#endif
