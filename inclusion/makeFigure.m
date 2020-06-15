
setPxLW(0.1)
setPxScale(300)
postTable('','stress_xx')
% rwbmap
% caxis([-3 3])
cb = colorbar;
cbt=get(cb,'title');
set(cbt,'string','\sigma_x_x')
set(cb,'position',[0.85,0.35,0.015,0.4])
set(gca,'position',[0.05 0.05 0.8 0.9])

set(gcf,'paperposition',[0 0 6 2.5])
print -depsc -opengl -r400 ~/teaching/5123/slides/3ptBendResult


