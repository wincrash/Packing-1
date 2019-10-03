#include "Mesh.h"

Mesh::Mesh(){
    
}
// Given segment pq and triangle abc, returns whether segment intersects
// triangle and if so, also returns the barycentric coordinates (u,v,w)
// of the intersection point
bool Mesh::SameSign(int x, int y)
{
    return (x >= 0) ^ (y < 0);
}
inline REAL Mesh::TriArea2D(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3)
{
return (x1-x2)*(y2-y3) - (x2-x3)*(y1-y2);
}
// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
Point Mesh::ClosestPtPointTriangle(Point p, Point a, Point b, Point c)
{
    // Check if P in vertex region outside A
                Point ab = b - a;
                Point ac = c - a;
                Point ap = p - a;
                float d1 = dot_prod(ab, ap);
                float d2 = dot_prod(ac, ap);
                if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)
                // Check if P in vertex region outside B
                Point bp = p - b;
                float d3 = dot_prod(ab, bp);
                float d4 = dot_prod(ac, bp);
                if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)
                // Check if P in edge region of AB, if so return projection of P onto AB
                float vc = d1*d4 - d3*d2;
                if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
                float v = d1 / (d1 - d3);
                return a + v * ab; // barycentric coordinates (1-v,v,0)
                }
                // Check if P in vertex region outside C
                Point cp = p - c;
                float d5 = dot_prod(ab, cp);
                float d6 = dot_prod(ac, cp);
                if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)142

                // Check if P in edge region of AC, if so return projection of P onto AC
                float vb = d5*d2 - d1*d6;
                if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
                float w = d2 / (d2 - d6);
                return a + w * ac; // barycentric coordinates (1-w,0,w)
                }
                // Check if P in edge region of BC, if so return projection of P onto BC
                float va = d3*d6 - d5*d4;
                if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
                float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
                return b + w * (c - b); // barycentric coordinates (0,1-w,w)
                }
                // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
                float denom = 1.0f / (va + vb + vc);
                float v = vb * denom;
                float w = vc * denom;
                return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}
void Mesh::Barycentric(Point a, Point b, Point c, Point p, REAL &u, REAL &v, REAL &w)
{
// Unnormalized triangle normal
				Point m = cross_prod(b - a, c - a);
				// Nominators and one-over-denominator for u and v ratios
                REAL nu, nv, ood;
				// Absolute components for determining projection plane
                REAL x = abs(m.x), y = abs(m.y), z = abs(m.z);
				// Compute areas in plane of largest projection
				if (x >= y && x >= z) {
				// x is largest, project to the yz plane
				nu = TriArea2D(p.y, p.z, b.y, b.z, c.y, c.z); // Area of PBC in yz plane
				nv = TriArea2D(p.y, p.z, c.y, c.z, a.y, a.z); // Area of PCA in yz plane
                ood = 1.0 / m.x; // 1/(2*area of ABC in yz plane)

				

				} else if (y >= x && y >= z) {
				// y is largest, project to the xz plane
				nu = TriArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
				nv = TriArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
                ood = 1.0 / -m.y;
				} else {
				// z is largest, project to the xy plane
				nu = TriArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
				nv = TriArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
                ood = 1.0 / m.z;
				}
				u = nu * ood;
				v = nv * ood;
                w = 1.0 - u - v;
}
int Mesh::PointInTriangle(Point p, Point a, Point b, Point c)
{
								// Translate point and triangle so that point lies at origin

            REAL u, v, w;
			Barycentric(a, b, c, p, u, v, w);
            return v >= 0.0 && w >= 0.0 && (v + w) <= 1.0;

				

}
int Mesh::IntersectLineTriangle(Point p, Point q, Point a, Point b, Point c){

    REAL u, v, w, t;
    Point r;
    Point ab = b - a;
			Point ac = c - a;
			Point qp = p - q;
			Point pq = q - p;
			Point pa = a - p;
			Point pb = b - p;
			Point pc = c - p;
			// Compute triangle normal. Can be precalculated or cached if
			// intersecting multiple segments against the same triangle
			Point n = cross_prod(ab, ac);
			// Compute denominator d. If d <= 0, segment is parallel to or points
			// away from triangle, so exit early
            REAL d = dot_prod(qp, n);
            if (d <= 0.0) return 0;
			// Compute intersection t value of pq with plane of triangle. A ray
			// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
			// dividing by d until intersection has been found to pierce triangle

			Point ap = p - a;
			t = dot_prod(ap, n);
			
            if (t < 0.0) return 0;
			if (t > d) return 0; // For segment; exclude this code line for a ray test
			// Compute barycentric coordinate components and test if within bounds
			if(t<0&& t>1)
				return 0;
			
			Point e = cross_prod(qp, ap);
			v = dot_prod(ac, e);
            if (v < 0.0 || v > d) return 0;
			w = -dot_prod(ab, e);
            if (w < 0.0 || v + w > d) return 0;
			// Segment/ray intersects triangle. Perform delayed division and
			// compute the last barycentric coordinate component
           // REAL ood = 1.0 / d;
            //t *= ood;
            //v *= ood;
            //w *= ood;
           // u = 1.0 - v - w;
            //r = u*a + v*b + w*c;
            //if(!PointInTriangle(r,  a, b, c))
            //return 0;



            return 1;
}
bool Mesh::check(Point newSphere){

        int count=0;
        // naudoti paprastus masyvus
        int susikirtimu_skaicius;
        Point q,a, q1;
        q.x=bounds[1]+(bounds[1]/2);
        q.y=bounds[3]+(bounds[3]/2);
        q.z=bounds[5]+(bounds[5]/2);
		//q.PrintStructure();
        //cout << " ribu reiksmes " << bounds[0] << " " << bounds[1] << " " << bounds[2] << " " << bounds[3] << " " << bounds[4] << " " << bounds[5] << endl;
       // newSphere.PrintStructure();
        //std::cout << "tikrinamas taskas " << endl;
      // Timer timer;
       //double rezultatai;
      // timer.StartTimer();
        REAL ilgis;

        for(int i=0, j=0;i<tria_kiekis;i++, j+=3){
            //cout << ilgis << " " << newSphere.R << "___________ " <<  endl;
            ilgis=vec_distance(newSphere, ClosestPtPointTriangle(newSphere, taskai[j], taskai[j+1], taskai[j+2]));
            //cout << ilgis << " " << newSphere.R << "___________ ======" <<  endl;
        //cout << ilgis <<  endl;
            
            
            ilgis=vec_distance(newSphere, ClosestPtPointTriangle(newSphere, taskai[j+2], taskai[j+1], taskai[j]));
       


        }


        for(int i=0, j=0;i<tria_kiekis;i++, j+=3){

           // cout << j <<"-------------- j reiksme " << " ---- ---- i reiksme >>> " << i <<  endl;
            if(newSphere.x+newSphere.R<bounds[1]&& newSphere.x-newSphere.R>bounds[0]&& newSphere.y+newSphere.R<bounds[3]&& newSphere.y-newSphere.R>bounds[2]&& newSphere.z+newSphere.R<bounds[5]&& newSphere.z-newSphere.R>bounds[4]){
                    if(IntersectLineTriangle(newSphere, q, taskai[j+2], taskai[j+1], taskai[j])){


                        count++;

                     }
				
                else if(IntersectLineTriangle(newSphere, q, taskai[j], taskai[j+1], taskai[j+2])){

                   count++;

               }





				
				
			    

                }
        }

       // cout << "daleles patikra baigta -------" << endl;
        //cout << "newSphere taskas ir susikirtimai " << count << " ";
      // newSphere.PrintStructure();
        //cout <<"----------" <<  endl;
        //timer.StopTimer();
        //rez+=timer.ElapsedTime("sec");
        //cout << rez<< " boundaries check laikas  " << endl;
       /* if(newSphere.x<bounds[0]||newSphere.x>bounds[1]||newSphere.y<bounds[2]||newSphere.y>bounds[3]||newSphere.z<bounds[4]||newSphere.z>bounds[5]&&count==1)
        {
           newSphere.PrintStructure();
           cout << "neigiama dalele su vienu kirtimu " << endl;
            count++;s
        }
        */
        //cout << count << " susikirtimu skaicius naujam suskaiciuotam taskui " << endl;
        //cout << "daleles reiksme ======" << endl;
        //newSphere.PrintStructure();
        //cout << "daleles reiksme ===== " << endl;
        //cout << "susikirtimu reiksme ===== " << count << " =========" <<  endl;


        if(count%2!=0)
		{
			
			//cout << " susikirtimu skaicius " << count << endl;
			//cout << "sfera kuri praeis" << endl;
			//newSphere.PrintStructure();
			return 1;
				
	
		}
        else return 0;
}
// susikirtimu skaiciavimas su tiese per erdve

void Mesh::initialization(json duomenys){

    std::string filename=duomenys["BOUNDARIES"]["FILENAME"];
    vtkDataSetReader* reader=vtkDataSetReader::New();
            reader->SetFileName(filename.c_str());
            reader->Update();

            bounds=reader->GetOutput()->GetBounds();

        //int tasku_kiekis=reader->GetOutput()->GetNumberOfPoints();
        tria_kiekis=reader->GetOutput()->GetNumberOfCells();
       //cout << tria_kiekis << "---------- trikampiu kiekis " << endl;
        double p[3];

        int pid1;
        int pid2;
        int pid3;
        //std::vector<REAL4> kruptis(tria_kikeis); galbut naudosiu per vtk susitvakryt
        //cout << "trikampiu kiekis----------- " << endl;
        for(int i=0;i<tria_kiekis;i++){
            //gauname cele kuri saugo i Pointo sujungimus
            vtkCell*cell=reader->GetOutput()->GetCell(i);
            //cout << cell->GetPointIds()->GetNumberOfIds() << endl;
            if(cell->GetPointIds()->GetNumberOfIds()==3){
                         pid1=cell->GetPointId(0);
                         pid2=cell->GetPointId(1);
                         pid3=cell->GetPointId(2);

          //  cout << pid1 << " " << pid2 <<  " " << pid3 << " pid reiksmes is eiles pagal trikampi " << endl;

            Point temp;
            reader->GetOutput()->GetPoint(pid1,p);
            temp.x=p[0];
            temp.y=p[1];
            temp.z=p[2];

            taskai.push_back(temp);

            reader->GetOutput()->GetPoint(pid2,p);
            temp.x=p[0];
            temp.y=p[1];
            temp.z=p[2];

            taskai.push_back(temp);
            reader->GetOutput()->GetPoint(pid3,p);
            temp.x=p[0];
            temp.y=p[1];
            temp.z=p[2];

            taskai.push_back(temp);
}
         }

      }
