// g++ surftrackexample.cpp libeltopo_release.a -llapack -lblas -lstdc++ -lm -I../common -I../eltopo3d -o  test 
//C hello world example
#include <stdio.h>
//#include <stdbool.h>
#include <math.h>

#include <vector>
#include <vec.h>
#include <surftrack.h>
#include <subdivisionscheme.h>

#define NSIZE 20000

#include "mesh.c"

int printmesh(int i, int Nverticies, int Ntriangles, double * verticies, int * triangles) {

  char buf[256];
  snprintf(buf, sizeof buf, "%s%d%s", "output/", i, ".obj");

  FILE *fp;

  fp = fopen(buf, "w+");

  //fprintf(fp, "This is testing for fprintf...\n");

  for (int i=0;i<Nverticies;i++){
    fprintf(fp,"v %f %f %f \n",verticies[3*i],verticies[3*i+1],verticies[3*i+2]);
  }

  for (int i=0;i<Ntriangles;i++) {
    fprintf(fp,"f %d %d %d\n",triangles[3*i]+1,triangles[3*i+1]+1,triangles[3*i+2]+1);
  }
    

  //  fputs("This is testing for fputs...\n", fp);
  fclose(fp);
}

int main()
{

  
  std::vector<Vec3d> vs;
  std::vector<double> masses;
    
  for ( int i = 0; i < Nverticies; ++i )
    {
      vs.push_back( Vec3d( verticies[3*i], verticies[3*i + 1], verticies[3*i + 2] ) );
      masses.push_back( 0.5 );      
    }
    
  std::vector<Vec3st> ts;
  for ( int i = 0; i < Ntriangles; ++i )
    {
      ts.push_back( Vec3st( triangles[3*i], triangles[3*i + 1], triangles[3*i + 2] ) );
    }
  
  SurfTrackInitializationParameters parameters;

  parameters.m_allow_non_manifold = false;
  parameters.m_allow_topology_changes = false;
  parameters.m_min_edge_length = 0.001; //0.5;
  parameters.m_max_edge_length = 0.2; // 1.5;
  parameters.m_max_volume_change = 0.1;
  parameters.m_subdivision_scheme = new ButterflyScheme();
  parameters.m_allow_vertex_movement = false;
  

  printmesh(0,Nverticies,Ntriangles,verticies,triangles);

  SurfTrack surface_tracker( vs, ts, masses, parameters );
  surface_tracker.m_verbose = false;

  //surface_tracker.improve_mesh();
  //surface_tracker.topology_changes();
  //surface_tracker.defrag_mesh();


  // Creating usual array objects
  
  int Nverticies_out = surface_tracker.get_num_vertices();
  double verticies_out[Nverticies_out*3];
  for ( int i = 0; i < Nverticies_out; ++i )
    {
        const Vec3d& pos = surface_tracker.get_position(i);
        verticies_out[3*i + 0] = pos[0];
        verticies_out[3*i + 1] = pos[1];
        verticies_out[3*i + 2] = pos[2];
    }

  int Ntriangles_out = surface_tracker.m_mesh.num_triangles();
  int triangles_out[Ntriangles_out];
  for ( int i = 0; i < Ntriangles_out; ++i )
    {
      const Vec3st& curr_tri = surface_tracker.m_mesh.get_triangle(i); 
      triangles_out[3*i + 0] = curr_tri[0];
      triangles_out[3*i + 1] = curr_tri[1];
      triangles_out[3*i + 2] = curr_tri[2];
    }


  printf("Nverticies=%d  Ntriangles=%d\n",Nverticies_out,Ntriangles_out);
  printmesh(100,Nverticies_out,Ntriangles_out,verticies_out,triangles_out);

        
  
  printf("Hello world\n");
  return 0;
}
