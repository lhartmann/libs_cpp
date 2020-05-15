function figsave(fig, file, varargin)
epspath = '.';
pngpath = '';

if nargin > 0
	for i=1:ceil((nargin-2)/2)
		opt = varargin{2*i-1};
		
		if      strcmp(opt, 'eps')
			epspath = varargin{2*i};
			
		elseif strcmp(opt, 'png')
			pngpath = varargin{2*i};

		end
	end
end

EPS = [epspath '/' file '.eps'];

if strcmp(pngpath, '')
	PNG = '';
else
	PNG = [pngpath '/' file '.png'];
end

PNG='';

%  ax  = get(fig, 'CurrentAxes')
%  lin = get(ax,  'Children')
%  for i=lin
%  	set(i,'LineWidth',2);
%  end

set(fig,'FileName',EPS);
filemenufcn(fig,'FileSave');

if PNG ~= ''
	system(['epstool --bitmap --dpi 300 "' EPS '" "temp.bmp"']);
	system(['convert -trim "temp.bmp" "' PNG '"']);
	system('rm -f "temp.bmp"');
end
