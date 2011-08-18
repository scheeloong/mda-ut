//============================================================================
// b r e s l i n e . c
//
// VERSION 3: draws from both ends and calculates single offset into FB.
//            (takes advantage of line symmetry). THIS VERSION SUFFERS FROM
//            THE MIDPOINT "BUMP" WHERE THE TWO ENDS MEET.
// Programmer:  Kenny Hoff
// Date:        10/25/95
// Purpose:     To implement the Bresenham's line drawing algorithm for all
//              slopes and line directions (using minimal routines). 
// http://www.cs.unc.edu/~hoff/projects/comp235/bresline/breslin3.txt
//============================================================================

//============================================================================
// Fills the intermediate points along a line between the two given endpoints
// using Bresenham's line drawing algorithm. NOTE: this routine does no clipping
// so the coordinate values must be within the FrameBuffer bounds.
//============================================================================
void draw_line(int Ax, int Ay, int Bx, int By, unsigned char Color, 
	       unsigned char* buf, int WIDTH, int HEIGHT, bool times3)
{
  unsigned char* FrameBuffer = buf;
  int fbXincr, fbYincr, fbXYincr, dPr, dPru, P;

  //------------------------------------------------------------------------
  // STORE THE FRAMEBUFFER ENDPOINT-ADDRESSES (A AND B)
  //------------------------------------------------------------------------
  unsigned char* AfbAddr = &FrameBuffer[Ay*WIDTH+Ax];
  unsigned char* BfbAddr = &FrameBuffer[By*WIDTH+Bx];
  
  //------------------------------------------------------------------------
  // STORE THE ABSOLUTE VALUE OF THE CHANGE IN X AND Y FOR THE LINE
  //------------------------------------------------------------------------
  int dX = Bx-Ax;
  int dY = By-Ay;
  if (dX >= 0) goto CheckDy;
  	dX = -dX;
  CheckDy: if (dY >= 0) goto dYOK;
    dY = -dY;
  dYOK:
	
  //------------------------------------------------------------------------
  // DETERMINE AMOUNT TO INCREMENT FRAMEBUFFER TO GET TO SUBSEQUENT POINTS
  //------------------------------------------------------------------------
  if (Ax > Bx) goto NEGX;      // DIRECTION IN X
    fbXincr=1; 
    goto AFTERNEGX;
  NEGX: 
    fbXincr=-1;
  AFTERNEGX:
    
  if (Ay > By) goto NEGY;      // DIRECTION IN Y
    fbYincr=WIDTH;            
    goto AFTERNEGY;
  NEGY:
    fbYincr=-WIDTH;
  AFTERNEGY:

  fbXYincr = fbXincr+fbYincr;  // DIRECTION IN X AND Y

  //------------------------------------------------------------------------
  // DETERMINE INDEPENDENT VARIABLE (ONE THAT ALWAYS INCREMENTS BY 1 (OR -1) )
  // AND INITIATE APPROPRIATE LINE DRAWING ROUTINE (BASED ON FIRST OCTANT
  // ALWAYS). THE X AND Y'S MAY BE FLIPPED IF Y IS THE INDEPENDENT VARIABLE.
  //------------------------------------------------------------------------
  if (dY > dX) goto YisIndependent;	// if Y is the independent variable
  
  // XisIndependent:
  	dPr	= dY<<1;      // amount to increment decision if right is chosen (always)
  	dPru = dPr-(dX<<1); // amount to increment decision if up is chosen
	P = dPr-dX;         // decision variable start value
      dX = dX>>1;         // COUNTER FOR HALF OF LINE (COMING FROM BOTH ENDS)
	XLOOP:  // PROCESS EACH POINT IN THE LINE ONE AT A TIME (use dX as counter)
      if(times3 == 0)
	{
		*AfbAddr = Color;              // plot the pixel A
		*BfbAddr = Color;              // plot the pixel B
        }
	else
	{
	  int p1 = 3*(AfbAddr-buf);
	  int p2 = 3*(BfbAddr-buf);
	  buf[p1+0] = Color;              // plot the pixel A
	  buf[p2+0] = Color;              // plot the pixel B
	  buf[p1+1] = 0;              // plot the pixel A
	  buf[p2+1] = 0;              // plot the pixel B
	  buf[p1+2] = 0;              // plot the pixel A
	  buf[p2+2] = 0;              // plot the pixel B
	}

		if (P > 0) goto RightAndUp;    // if the pixel going right AND up?
		// Up:
			AfbAddr+=fbXincr;
			BfbAddr-=fbXincr;
			P+=dPr;                  // increment decision (for right)
			dX--;
			if (dX>=0) goto XLOOP;
			return;			
		RightAndUp:
			AfbAddr+=fbXYincr;
			BfbAddr-=fbXYincr;
			P+=dPru;                 // increment decision (for up)
			dX--;
			if (dX>=0) goto XLOOP;
			return;			
			
  YisIndependent:
	dPr = dX<<1;        // amount to increment decision if right is chosen (always)
	dPru = dPr-(dY<<1); // amount to increment decision if up is chosen
	P = dPr-dY;         // decision variable start value
      dY = dY>>1;         // COUNTER FOR HALF OF LINE (COMING FROM BOTH ENDS)
	YLOOP:  // PROCESS EACH POINT IN THE LINE ONE AT A TIME (use dY as counter)
      if(times3 == 0)
	{
	  *AfbAddr = Color;              // plot the pixel A
	  *BfbAddr = Color;              // plot the pixel B
	}
      else
	{
	  int p1 = 3*(AfbAddr-buf);
	  int p2 = 3*(BfbAddr-buf);
	  buf[p1+0] = Color;              // plot the pixel A
	  buf[p2+0] = Color;              // plot the pixel B
	  buf[p1+1] = 0;              // plot the pixel A
	  buf[p2+1] = 0;              // plot the pixel B
	  buf[p1+2] = 0;              // plot the pixel A
	  buf[p2+2] = 0;              // plot the pixel B
	}
		if (P > 0) goto RightAndUp2;   // if the pixel going right AND up?
		// Up:
			AfbAddr+=fbYincr;
			BfbAddr-=fbYincr;
			P+=dPr;                  // increment decision (for right)
			dY--;
			if (dY>=0) goto YLOOP;
			return;			
		RightAndUp2:
 			AfbAddr+=fbXYincr;
 			BfbAddr-=fbXYincr;
			P+=dPru;                 // increment decision (for up)
			dY--;
			if (dY>=0) goto YLOOP;
			return;			
}



