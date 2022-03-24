require 'optparse'


class Numeric
  def to_hex
    "0x#{self.to_s(16).rjust(2, '0')}"
  end
end


class Random
  def dice ratio
    norm = ratio.reduce(&:+)
    p = self.rand
    s = 0.0
    ratio.each.with_index do |r, i|
      s += r.to_f / norm
      return i if p < s
    end
    length(ratio) - 1
  end
end


NUM_SUBCELLS = 4

class FieldGenerator

  def initialize(cellx, celly, wall, seed=nil)
    @cellx = cellx
    @celly = celly
    @wall  = wall
    @rnd   = Random.new
  end

  def gen_field(ratio)
    fields = gen_field_withou_wall(ratio)
    fields = add_walls(fields) if @wall
    fields.map do |field|
      {
        address: "cell:#{field[:x]}-#{field[:y]}",
        data: field[:data].map(&:to_hex).join("\n")
      }
    end
  end

  private
  def gen_field_withou_wall(ratio)
    (0...@cellx).to_a.product((0...@celly).to_a).map do |x, y|
      {
        x: x, y: y,
        data: NUM_SUBCELLS.times.map{|_| @rnd.dice(ratio)}
      }
    end
  end

  def add_walls(fields)
    fields.map do |field|
      data = field[:data]
      case field[:x]
      when 0
        data[0] = (data[0] | 0x10)
        data[2] = (data[2] | 0x10)
      when (@cellx - 1)
        data[1] = (data[1] | 0x10)
        data[3] = (data[3] | 0x10)
      end
      case field[:y]
      when 0
        data[2] = (data[2] | 0x10)
        data[3] = (data[3] | 0x10)
      when (@celly - 1)
        data[0] = (data[0] | 0x10)
        data[1] = (data[1] | 0x10)
      end
      {
        x: field[:x], y: field[:y], data: data
      }
    end
  end

end


options = {
  ratio: [70,30],
  cellx: 10,
  celly: 20,
  wall:  false
}

class << options
  def gen_header
    [
      "# Timestamp: #{Time.now}",
      "# Opinions: #{self[:ratio].length}, ratio = #{self[:ratio]}",
      "# Size: x = #{self[:cellx]}, y = #{self[:celly]}",
      "# Wall: #{self[:wall]}"
    ].join("\n")
  end
end


if __FILE__ == $0

  OptionParser.new do |opt|
    opt.on('-r=R,R,...', '--ratio=R,R,...', 'ratio of each opinion'){|r| options[:ratio] = r.split(",").map(&:to_f)}
    opt.on('--num-cellx=V', 'the number of cells on X-axis'){|v| options[:cellx] = v.to_i}
    opt.on('--num-celly=V', 'the number of cells on Y-axis'){|v| options[:celly] = v.to_i}
    opt.on('-w', '--[no-]wall', 'flag to generate walls'){|f| options[:wall] = f}
    opt.parse!(ARGV)
  end

  puts options.gen_header
  puts ""
  puts FieldGenerator.new(options[:cellx], options[:celly], options[:wall]).gen_field(options[:ratio]).map{|field|
    [
      "address",
      field[:address],
      "",
      "data",
      field[:data]
    ].join("\n")
  }.join("\n\n")

end
