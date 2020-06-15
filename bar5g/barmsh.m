% function barmsh
% generate mesh for bar
% one element in width direction

nx = 11;    % number of elements in x-direction
L = 20;      % length
W = 10;     % width

ieMid = ceil(nx/2);
nx0 = nx-1;  
dx = L / nx0;
x0 = L/2+dx/2;
xs = -x0:dx:x0;
ys = [-.5 .5]*W; % relative y-mesh coordinates

nnx = length(xs);
ws = zeros(1,nnx);
ny = 1;

% nodal coordinates 2d
nn = 0;
x = zeros(1,(nx+1)*(ny+1));
y = zeros(1,(nx+1)*(ny+1));
for ix=0:nx
  for iy=0:ny
    nn = nn+1;
    y(nn) = ys(iy+1);
    x(nn) = xs(ix+1);
  end
end

% element connectivity 2d
nel2d=nx*ny;
el2d=zeros(nel2d,4);
iel = 0;
for ix=1:nx  
  n0=(ix-1)*(ny+1);
  for iy=1:ny
    iel = iel + 1;
    el2d(iel,1)=n0+iy;
    el2d(iel,2)=el2d(iel,1)+ny+1;
    el2d(iel,3)=el2d(iel,1)+ny+2;
    el2d(iel,4)=el2d(iel,1)+1;
  end
end

% plot 2dmesh
clf
for iel=1:nel2d
  in = el2d(iel,:);
  patch(x(in),y(in),x(in),'linewidth',0.1);
end
set(gca,'dataaspectRatio',[1 1 1])
axis off

% nodal coordinates 3d
nnz = 1;
zk = [0];
coord = zeros(nnz*nn,4);
for i=1:nn
  for k=1:nnz
    coord((k-1)*nn+i,1)=(k-1)*nn+i;
    coord((k-1)*nn+i,2)=x(i);
    coord((k-1)*nn+i,3)=y(i);
    coord((k-1)*nn+i,4)=zk(k);
  end
end

% write mesh to file
delete bar.mesh
fid=fopen('bar.mesh','w');

fprintf(fid,'<Nodes>\n');
  fprintf(fid,'%6.0f %12.8f %12.8f; \n',coord(:,1:3)'); % no z-coordinates
fprintf(fid,'</Nodes>\n\n');

fprintf(fid,'<Elements>\n');
elnums = zeros( nel2d , 1 );
elnums = (1:nel2d)';
fprintf(fid,'%i %i %i %i %i; \n',[elnums el2d]');
fprintf(fid,'</Elements>\n\n');

% solid elements
ieelastic = elnums(elnums~=ieMid);
fprintf(fid,['<ElementGroup name="elastic">\n{']);
fprintf(fid,'%i',ieelastic(1));
fprintf(fid,',%i',ieelastic(2:end));
fprintf(fid,'}\n</ElementGroup>\n\n');
fprintf(fid,['<ElementGroup name="damage">\n{']);
fprintf(fid,'%i}\n</ElementGroup>\n\n',ieMid);
% 
% % interface elements
% for k=1:nnz-1
%   fprintf(fid,['<ElementGroup name="interface' num2str(k) '">\n{']);
%   fprintf(fid,'%4.0f',elnums(1,k+nnz));
%   fprintf(fid,',%4.0f',elnums(2:nel2d,k+nnz));
%   fprintf(fid,'}\n</ElementGroup>\n\n');
% end

