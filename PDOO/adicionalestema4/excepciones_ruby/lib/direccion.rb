#encoding: utf-8


class Direccion
  attr_reader :calle, :numero, :codigoPostal
  attr_accessor :ciudad
  def initialize(calle, numero, cp, ciudad)
    @calle = calle
    @numero = numero
    @codigoPostal = cp
    @ciudad = ciudad    
  end
  # Método en el que se lanza una excepción
  def codigoPostal(cp)
    if (cp.size <=> 5)  
      raise "código postal erroneo"
    end
    @codigoPostal = cp
  end
 
end
