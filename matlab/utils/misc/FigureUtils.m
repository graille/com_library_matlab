classdef FigureUtils
  methods(Static)
    function savefig(dirpath_o, filename, shape, create_directory, suffix)
      % Remove special characters from filename
      filename = strrep(filename, '.', '_');
      filename = strrep(filename, ',', '_');
      filename = strrep(filename, ' ', '');
      filename = strrep(filename, '\t', '');

      if nargin < 3, shape = 'all'; end
      if nargin < 4, create_directory = true; end
      if nargin < 5 || isempty(suffix), suffix = ''; end
      
      if create_directory
        dirpath = fullfile(dirpath_o, filename);
      else
        dirpath = dirpath_o;
      end
      
      switch lower(shape)
        case 'square'
          dims = [1 1];
        case 'rectangle'
          dims = [(1+sqrt(5))/2 1];
        case 'all'
          FigureUtils.savefig(dirpath_o, filename, 'square', create_directory, 'square');
          FigureUtils.savefig(dirpath_o, filename, 'rectangle', create_directory, 'rect');
          return
        otherwise
          FigureUtils.savefig(dirpath_o, filename);
          return
      end
      
      %% Configure figure properties
      dims = dims .* 600;
      old_figure_position = get(gcf, 'Position');
      
      set(gcf, 'Position', [10 10 dims(1) dims(2)]);
      
      % Axis dimensions
      % Source: https://fr.mathworks.com/help/releases/R2019b/matlab/creating_plots/save-figure-with-minimal-white-space.html
      ax = gca;
      
      outerpos = ax.OuterPosition;
      ti = ax.TightInset; 
      left = outerpos(1) + ti(1);
      bottom = outerpos(2) + ti(2);
      ax_width = outerpos(3) - ti(1) - ti(3) - 0.001;
      ax_height = outerpos(4) - ti(2) - ti(4) - 0.001;
      
      old_ax_position = ax.Position;
      ax.Position = [left bottom ax_width ax_height];
      
      %% Save
      if ~exist(dirpath_o , 'dir'), mkdir(dirpath_o); end
      if ~exist(dirpath , 'dir'), mkdir(dirpath); end
      
      % Add suffix if needed
      if ~isempty(suffix)
        filename = sprintf('%s_%s', filename, suffix);
      end
      
      % Set paper size for PDF files
      set(gcf, 'Units', 'Inches');
      pos = get(gcf, 'Position');
      set(gcf, 'PaperPositionMode', 'Auto', 'PaperUnits', 'Inches', 'PaperSize', [pos(3), pos(4)])
      
      saveas(gcf, fullfile(dirpath, sprintf('%s.fig', filename)));
      saveas(gcf, fullfile(dirpath, sprintf('%s.png', filename)));
      saveas(gcf, fullfile(dirpath, sprintf('%s.pdf', filename)));
      
      % Restore parameters
      set(gcf, 'Units', 'Pixels');
      set(gca, 'Position', old_ax_position);
      set(gcf, 'Position', old_figure_position);
      
      fprintf('Figure %s saved \n', filename);
    end
  end
end