A= imread('C:\Users\hp\Downloads\map.bmp');

BW = im2bw(A,0.1);
fid = fopen('Mymatrix.txt','wt');
for ii = 1:size(BW,1)
    fprintf(fid,'%g ',BW(ii,:));
    fprintf(fid,'\n');
end
fclose(fid)
imshow(A), figure, imshow(BW)