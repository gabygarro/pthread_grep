		      FaceSaver Format Revealed


A FaceSaver image is an ASCII file containing some or all of the
following lines:

   FirstName:
   LastName:
   E-mail:
   Telephone:
   Company:
   Address1:
   Address2:
   CityStateZip:
   Date:
   PicData:  Actual data:  width - height - bits/pixel
   Image:    Should be transformed to: width - height - bits/pixel
   (A REQUIRED Blank line)

   Hexified  picture  in scanline  order,  in the  form   suitable for
printing in  postscript. It  is unpadded scanlines,  I believe left to
right.  If it is bottom to top it is due to funny  transforms in going
from the real scanned image, which came from a  video camera turned on
its side!   (to get  a portrait aspect  ratio).  All  the images are 8
bits  per pixel, and have  been intensity levelled to  use up the full
dynamic range (each pixel  multiplied by 256/(max-min) and  shifted so
that min becomes zero).  I'm  sorry about that, I  should have put out
untransformed data.  The original frame grabbing  was  off a Targa M8,
and  the  gamma varied due  to a number  of real  world factors.  Most
pictures had a dynamic range in the 128--196 vicinity.

The line labeled Image: can be used to correct  for non-square pixels.
In most cases, there are  108 (non-square) pixels  across in the data,
but  they would have   been 96  pixels  across   if they were  square.
Therefore, Image: says 96, PicData says 108.

Lou Katz
Saver of Lost Faces

lou@usenix.org
(415) 530-8870


 